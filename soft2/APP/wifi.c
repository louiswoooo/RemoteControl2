/*****************************************************************************
  * @file:    	wifi.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-4-17
  * @brief:	控制wifi 模块esp8266 ，为上层提供接口
*******************************************************************************/
#include "usart.h"
#include "string.h"
#include "gpio.h"
#include "sys.h"
#include "wifi.h"
#include "http_ap.h"
#define SERVER_ADDR		"119.3.233.56"
#define SERVER_PORT		8000

#define WIFI_CMD_TIMEOUT_MS	200
#define WIFI_CMD_TRY_TIMES		5

#define WIFI_SEND_TIMEOUT_MS	500
#define WIFI_SEND_TRY_TIMES	3

typedef	struct	
{
	u8 *send;			//命令
	u8 *match;			//匹配
}
_t_WIFI_CMD_Info;

/*****************************************************************************
ESP8266配置变量
*******************************************************************************/
//恢复出厂设置
const _t_WIFI_CMD_Info	CMD_Restore={"AT+RESTORE\r\n", "OK"};                
//##配置ESP8266模块为AP模式，开启wifi热点。
const _t_WIFI_CMD_Info	CMD_APMode={"AT+CWMODE=2\r\n", "OK"};   
//wifi名：ESP8266。密码：123456。4是加密方式。4是信道
const _t_WIFI_CMD_Info	CMD_SSID={"AT+CWSAP=\"AI_Lab\",\"1234567890\",4,4\r\n", "OK"};                
//##重启模块使AP模式生效
const _t_WIFI_CMD_Info	CMD_Reset={"AT+RST\r\n", "OK"};                      
//## 1启动多连接，0关闭
//这里强调一下，ESP8266作为服务器做多支持5个客户端的链接，id分配顺序是0-4。
const _t_WIFI_CMD_Info	CMD_Muti={"AT+CIPMUX=1\r\n", "OK"};                
//##模块开启服务器模式，端口号8080
const _t_WIFI_CMD_Info	CMD_Server={"AT+CIPSERVER=1,5000\r\n", "OK"};        
//##查看一下ESP8266的IP
const _t_WIFI_CMD_Info	CMD_IP={"AT+CIFSR\r\n", "OK"};        

//ap模式下的发送命令，0连接客户端，170发送字节长度
_t_WIFI_CMD_Info 	CMD_Send= {"AT+CIPSEND=0,170\r\n", "OK"};


#define WIFI_GPIO_PORT		GPIO_P5
#define WIFI_GPIO_PIN		GPIO_Pin_3

#define	wifi_reset_pin_low()		GPIO_ClearBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)
#define	wifi_reset_pin_high()		GPIO_SetBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)

/*
  * @brief:	整形数字转换成字符串函数
  * @param:	num 要转换的整数，str 转换出来的字符串的存贮控件指针
  * @retval:	返回去掉前面的0的整数字符串指针
*/
u8 *int_to_str( u8 *str,u16 num)
{
	u8 *p;
	str[0] = num/10000+0x30;
	str[1] = num%10000/1000+0x30;
	str[2] = num%1000/100+0x30;
	str[3] = num%100/10+0x30;
	str[4] = num%10+0x30;
	str[5] = '\0';
	p=str;
	while(*p == '0')
		p++;
	if(*p == 0)
		p++;
	return p;
}

//esp8266硬件重启
void wifi_reset(void)
{
	GPIO_InitTypeDef	wifi_gpio_init;

	wifi_gpio_init.Mode=GPIO_PullUp;
	wifi_gpio_init.Pin=WIFI_GPIO_PIN;
	
	GPIO_Inilize(WIFI_GPIO_PORT, &wifi_gpio_init);

	wifi_reset_pin_low();
	delay_ms(100);
	wifi_reset_pin_high();
	delay_s(2);
}
/****************************************************************************************
  * @brief:	esp8266 串口接收函数，启动接收，以块为单位接收串口信息，
			调用函数以后，如果TimeOutSet2*(ms/tick) 时间内接收不到新字节
			接收块结束，返回接收到的字节数，本函数为阻塞函数	
			阻塞时间为: 接收块时间+ TimeOutSet2
  * @param:	none
  * @retval:	返回接收到的字节数，接收到的块存在缓存RX2_Buffer
*****************************************************************************************/
u8 wifi_receive()
{
	COM2.B_RX_EN=1;
	COM2.RX_Cnt=0;
	COM2.B_RX_OK=0;
	COM2.RX_TimeOut=TimeOutSet2;
	while(COM2.RX_TimeOut>0);			//每次串口接收不到字节产生超时，判定为一次块接收完成
	COM2.B_RX_EN=0;
	if(COM2.B_RX_OK)
	{
		return COM2.RX_Cnt;
	}
	return NULL;
}


/****************************************************************************************
  * @brief:	给esp8266 发送命令，并匹配缓冲区当中的字符				
  * @param:	send:	要发送的命令
			match:	要匹配的字符串
			timeout_ticks:	设置超时的时间ms
  * @retval:	如果在超时时间内收到多条消息，每条消息都与match 匹配，
  			成功立即返回1，超时则返回0
*****************************************************************************************/

u8 WIFI_SendAndWait(u8 *send, u8 *match, u16 timeout_ms)
{
	u8 *p;
	u8 i;
	Usart2SendString(send);			//发送命令
	for(i = timeout_ms/(TimeOutSet2 * MS_PER_TICK); i>0; i--)		//计算超时次数
	{
		if(wifi_receive())			//等待块消息
		{
			debug(RX2_Buffer);
			if(p=strstr(RX2_Buffer,match))
			{
				debug_vip("match");
				return 1;
			}
		}
	}
	return 0;
}
/****************************************************************************************
  * @brief:	客户端连接 发送函数，先发送准备发送的命令，AT+CIPSEND=clientid,length	，
  			参数是client的号（0-4），和发送数据的长度，然后才是内容，
  			如果length大于实际发送的数据长度，则此次发送不发生，
  			如果length小于实际发送的长度，则此次发送length长度的数据，多余的数据截断丢失
  * @param:	client_id，连接的客户端id， p发送内容指针
  * @retval:	成功返回1，失败0
*****************************************************************************************/
u8 send_to_client(u8 *client_id, u8 *p)
{
	u8 temp[6];
	u8 cmd[30]="AT+CIPSEND=";
	u8 *str;
	u16 send_len = strlen(p);
	strcat(cmd, client_id);				//组合客户端id
	strcat(cmd, ",");
	str = int_to_str(temp, send_len);
	strcat(cmd, str);					//组合长度
	strcat(cmd, "\r\n");

	if( !WIFI_SendAndWait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))		//发送发送命令
		return FAIL;
	if( !WIFI_SendAndWait(p, "SEND OK", WIFI_SEND_TIMEOUT_MS))			//发送内容
		return FAIL;

	return SUCCESS;
}
/****************************************************************************************
  * @brief:	http送函数，先发送发送http 头，然后是content_length，最后content
  * @param:	client_id，连接的客户端id， p发送内容指针
  * @retval:	成功返回1，失败0
*****************************************************************************************/
u8 http_send(u8 *client_id, u8 *content)
{
	u8 http_content_lenth_string[10];
	u8 temp[6];
	u8 *p;
	u16 content_size = strlen(content);
	
	memset(http_content_lenth_string, 0, sizeof(http_content_lenth_string));
	if(!send_to_client(client_id, HTTP_H1) )		//发送http 头，不包括content_length
		return FAIL;
	
	p = int_to_str( temp, content_size);			//发送content_length和两个换行符
	strcat(http_content_lenth_string, p);
	strcat(http_content_lenth_string,"\r\n\r\n");
	if(!send_to_client(client_id, http_content_lenth_string))
		return FAIL;
	
	if(!send_to_client(client_id, content))		//发送内容
		return FAIL;
	return SUCCESS;

}



u8 WIFI_Set_AP_mode(void)
{
	if( !WIFI_SendAndWait(CMD_APMode.send, CMD_APMode.match, WIFI_CMD_TIMEOUT_MS))
		return FAIL;
	if( !WIFI_SendAndWait(CMD_SSID.send, CMD_SSID.match, WIFI_CMD_TIMEOUT_MS ))
	{
		delay_s(2);
		return FAIL;
	}
	if( !WIFI_SendAndWait(CMD_Reset.send, CMD_Reset.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	delay_s(2);
	if( !WIFI_SendAndWait(CMD_Muti.send, CMD_Muti.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	if( !WIFI_SendAndWait(CMD_Server.send, CMD_Server.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	if( !WIFI_SendAndWait(CMD_IP.send, CMD_IP.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	return SUCCESS;
}


