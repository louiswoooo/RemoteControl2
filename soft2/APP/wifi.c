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
#define SERVER_ADDR		"119.3.233.56"
#define SERVER_PORT		8000

typedef	struct	
{
	u8 *send;			//命令
	u8 *match;			//匹配
	u8 timeout_ticks;	//超时
}
_t_WIFI_CMD_Info;

/*****************************************************************************
ESP8266配置变量
*******************************************************************************/
//恢复出厂设置
const _t_WIFI_CMD_Info	AP_Para0={"AT+RESTORE\r\n", "OK", 300};                
//##配置ESP8266模块为AP模式，开启wifi热点。
const _t_WIFI_CMD_Info	AP_Para1={"AT+CWMODE=2\r\n", "OK", 300};   
//wifi名：ESP8266。密码：123456。4是加密方式。4是信道
const _t_WIFI_CMD_Info	AP_Para2={"AT+CWSAP=\"AI_Lab\",\"1234567890\",4,4\r\n", "OK", 300};                
//##重启模块使AP模式生效
const _t_WIFI_CMD_Info	AP_Para3={"AT+RST\r\n", "OK", 300};                      
//## 1启动多连接，0关闭
//这里强调一下，ESP8266作为服务器做多支持5个客户端的链接，id分配顺序是0-4。
const _t_WIFI_CMD_Info	AP_Para4={"AT+CIPMUX=1\r\n", "OK", 300};                
//##模块开启服务器模式，端口号8080
const _t_WIFI_CMD_Info	AP_Para5={"AT+CIPSERVER=1,5000\r\n", "OK", 300};        
//##查看一下ESP8266的IP
const _t_WIFI_CMD_Info	AP_Para6={"AT+CIFSR\r\n", "OK", 300};        

//ap模式下的发送命令，0连接客户端，170发送字节长度
_t_WIFI_CMD_Info 	AP_Send_Para= {"AT+CIPSEND=0,170\r\n", "OK", 300};


#define WIFI_GPIO_PORT		GPIO_P5
#define WIFI_GPIO_PIN		GPIO_Pin_3

#define	wifi_reset_pin_low()		GPIO_ClearBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)
#define	wifi_reset_pin_high()		GPIO_SetBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)

u8 *int_to_str(u16 num)
{
	u8 *p;
	static u8 str[6];
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
	delay_s(5);
}
/****************************************************************************************
  * @brief:	esp8266 发送函数，先发送准备发送的命令，AT+CIPSEND=clientid,length	，
  			参数是client的号（0-4），和发送数据的长度，然后才是内容，
  			如果length大于实际发送的数据长度，则此次发送不发生，
  			如果length小于实际发送的长度，则此次发送length长度的数据，多余的数据截断丢失
  * @param:	none
  * @retval:	返回接收到的字节数，接收到的块存在缓存RX2_Buffer
*****************************************************************************************/
u8 wifi_send(u8 *p)
{
	u8 cmd[30]="AT+CIPSEND=0,";
	u8 *str;
	str = int_to_str((u16)strlen(p));
	strcat(cmd, str);
	strcat(cmd, "\r\n");
	debug("------------>>>>");
	debug(cmd);
	debug("$$$$$$$$$$$$$$$$$$$$$$$\r\n");
	AP_Send_Para.send = cmd;
	AP_Send_Para.match = "> ";
	AP_Send_Para.timeout_ticks = 300;
	if( !WIFI_SendAndWait(AP_Send_Para.send, AP_Para1.match, AP_Para1.timeout_ticks))		//发送发送命令
		return FAIL;
	if( !WIFI_SendAndWait(p, "OK", 300))			//发送内容
		return FAIL;
	return SUCCESS;
}
/****************************************************************************************
  * @brief:	esp8266 串口接收函数，启动接收，以块为单位接收串口信息，
			调用函数以后，如果TimeOutSet2 时间内接收不到新字节
			接收块结束，返回接收到的字节数，本函数为阻塞函数	
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
	if(COM2.B_RX_OK)
	{
		COM2.B_RX_EN=0;
		return COM2.RX_Cnt;
	}
	return NULL;
}

/****************************************************************************************
  * @brief:	给esp8266 发送命令，并匹配缓冲区当中的字符				
  * @param:	send:	要发送的命令
			match:	要匹配的字符串
			timeout_ticks:	设置超时的ticks
  * @retval:	如果与match 匹配，则返回匹配指针，否则返回NULL
*****************************************************************************************/

u8 *WIFI_SendAndWait(u8 *send, u8 *match, u16 timeout_ticks)
{
	u8 *p;
	u16 i;
	for(i = timeout_ticks/TimeOutSet2; i>0; i--)
	{
		Usart2SendString(send);
		if(wifi_receive())
		{
			debug(RX2_Buffer);
			if(p=strstr(RX2_Buffer,match))
				return 1;
		}
	}
	return 0;
}

u8 WIFI_Set_AP_mode(void)
{/*
	if( !WIFI_SendAndWait(AP_Para0.send, AP_Para0.match, AP_Para0.timeout_ticks))
		return FAIL;
	delay_s(5);
	*/
	if( !WIFI_SendAndWait(AP_Para1.send, AP_Para1.match, AP_Para1.timeout_ticks))
		return FAIL;
	if( !WIFI_SendAndWait(AP_Para2.send, AP_Para2.match, AP_Para2.timeout_ticks))
	{
		delay_s(2);
		return FAIL;
	}
	if( !WIFI_SendAndWait(AP_Para3.send, AP_Para3.match, AP_Para3.timeout_ticks))
		return FAIL;
	delay_s(5);
	if( !WIFI_SendAndWait(AP_Para4.send, AP_Para4.match, AP_Para4.timeout_ticks))
		return FAIL;
	if( !WIFI_SendAndWait(AP_Para5.send, AP_Para5.match, AP_Para5.timeout_ticks))
		return FAIL;
	if( !WIFI_SendAndWait(AP_Para6.send, AP_Para6.match, AP_Para6.timeout_ticks))
		return FAIL;
	return SUCCESS;
}

/*
u8 WIFI_Set_Client_mode(void)
{
	

}
*/

