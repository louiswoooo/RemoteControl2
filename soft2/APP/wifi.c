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
#include "http_text.h"
#define SERVER_ADDR		"119.3.233.56"
#define SERVER_PORT		8000

#define WIFI_CMD_TIMEOUT_MS	3000
#define WIFI_CMD_TRY_TIMES		5

#define WIFI_SEND_TIMEOUT_MS	3000
#define WIFI_SEND_TRY_TIMES	3

typedef	struct	
{
	u8 *send;			//命令
	u8 *match;			//匹配
}
_t_WIFI_CMD_Info;

/*****************************************************************************
设置ESP8266 服务端模式命令和答复关键字
连接流程:设置AP模式-设置ssid和密码-重启-设置多链接-开启服务器-查看ip
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

/*****************************************************************************
设置ESP8266 客户端 模式命令和答复关键字
连接流程:设置STA模式-重启-连接wifi-开启单链接-建立TCP连接
*******************************************************************************/
//##配置ESP8266模块为Station 模式，开启wifi热点。
const _t_WIFI_CMD_Info	CMD_STAMode={"AT+CWMODE=1\r\n", "OK"};   
//连接路由器，wifi名：HiTV_woo。密码：1234567890。
const _t_WIFI_CMD_Info	CMD_Connect_WIFI={"AT+CWJAP=\"HiTV_woo_plus\",\"1234567890\"\r\n", "OK"};                
//开启单链接
const _t_WIFI_CMD_Info	CMD_Single={"AT+CIPMUX=0\r\n", "OK"};                
//建立TCP 连接
const _t_WIFI_CMD_Info	CMD_Connect_Server={"AT+CIPSTART=\"TCP\",\"192.168.1.106\",5000\r\n", "OK"};        
//关闭TCP 连接
const _t_WIFI_CMD_Info	CMD_Disconnect_Server={"AT+CIPCLOSE=0\r\n", "OK"};        

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
void WIFI_Reset(void)
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
  * @param:	timeout_ms, wifi 接收的超时时间
  * @retval:	返回接收到的字节数，接收到的块存在缓存RX2_Buffer
*****************************************************************************************/
u8 WIFI_Receive(u16 timeout_ms)	
{
	u16 i;
	if(timeout_ms > 0)
	{
		for(i= timeout_ms/(TimeOutSet2* MS_PER_TICK); i>0; i--)
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
		}
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
u8 wifi_send_and_wait(u8 *send, u8 *match, u16 timeout_ms)
{
	u8 *p;
	u16 i;
	
	Usart2SendString(send);			//发送命令
	for(i = (timeout_ms/TimeOutSet2) / MS_PER_TICK; i>0; i--)		
	{
		if(WIFI_Receive(TimeOutSet2 * MS_PER_TICK))			//等待块消息
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
  * @brief:	esp8266作为服务端向连接的客户端发送函数，格式如下:
  			AT+CIPSEND=clientid,length		//client 连接的客户端id，length 发送的数据长度
  			OK
  			>xxxxxxx				//发送的数据
  			busy s...
  			Recv xx bytes
  			SEND OK
  			
  			如果length大于实际发送的数据长度，则此次发送不发生，
  			如果length小于实际发送的长度，则此次发送length长度的数据，多余的数据截断丢失
  * @param:	client_id，连接的客户端id， p发送内容指针
  * @retval:	成功返回1，失败0
*****************************************************************************************/
u8 server_send_to_client(u8 *client_id, u8 *p)
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

	if( !wifi_send_and_wait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))		//发送发送命令
		return FAIL;
	if( !wifi_send_and_wait(p, "SEND OK", WIFI_SEND_TIMEOUT_MS))			//发送内容
		return FAIL;

	return SUCCESS;
}
/****************************************************************************************
  * @brief:	esp8266作为客户端向连接的服务端发送函数，格式如下:
  			AT+CIPSEND=length		//length 发送的数据长度
  			OK
  			>xxxxxxx				//发送的数据
  			busy s...
  			Recv xx bytes
  			SEND OK
  			
  			如果length大于实际发送的数据长度，则此次发送不发生，
  			如果length小于实际发送的长度，则此次发送length长度的数据，多余的数据截断丢失
  * @param:	p发送内容指针
  * @retval:	成功返回1，失败0
****************************************************************************************
u8 client_send_to_server( u8 *p)
{
	u8 temp[6];
	u8 cmd[30]="AT+CIPSEND=";
	u8 *str;
	u16 send_len = strlen(p);
	str = int_to_str(temp, send_len);
	strcat(cmd, str);					//组合长度
	strcat(cmd, "\r\n");

	if( !wifi_send_and_wait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))		//发送发送命令
		return FAIL;
	if( !wifi_send_and_wait(p, "SEND OK", WIFI_SEND_TIMEOUT_MS))			//发送内容
		return FAIL;

	return SUCCESS;
}
*/
/****************************************************************************************
  * @brief:	esp8266作为服务端的http response 函数，收到http请求以后，
  			调用该函数进行response (参看HTTP 协议)
  			分成3部分发送，先发送http 头，然后是content_length，最后content
  * @param:	client_id，连接的客户端id， p发送内容指针
  * @retval:	成功返回1，失败0
*****************************************************************************************/
u8 WIFI_Server_HTTP_Response(u8 *client_id, u8 *content)
{
	u8 http_content_lenth_string[10];
	u8 temp[6];
	u8 *p;
	u16 content_size = strlen(content);
	
	memset(http_content_lenth_string, 0, sizeof(http_content_lenth_string));
	if(!server_send_to_client(client_id, HTTP_Server_Response_Head1) )		//发送http 头，不包括content_length
		return FAIL;
	
	p = int_to_str( temp, content_size);			//发送content_length和两个换行符
	strcat(http_content_lenth_string, p);
	strcat(http_content_lenth_string,"\r\n\r\n");
	if(!server_send_to_client(client_id, http_content_lenth_string))
		return FAIL;
	
	if(!server_send_to_client(client_id, content))		//发送内容
		return FAIL;
	return SUCCESS;

}
/****************************************************************************************
  * @brief:	esp8266作为客户端的http request 函数，发送服务器http请求
  			调用该函数进行request (参看HTTP 协议)
  			分成4部分发送:
  			1. 发送http 头前导,例如: GET /a11?
  			2. 发送请求参数SWITH1=ON&SWITCH2=OFF&xxxx，
  			3. 发送头剩余信息
  			4. 发送步骤2的参数
  * @param:	h1 http 头前导, http参数，3头部剩余
  * @retval:	成功返回1，失败0
*****************************************************************************************/
u8 WIFI_Client_HTTP_Request(u8 *h1, u8 *para, u8 *h3)
{
	u8 temp[6];
	u8 cmd[30]="AT+CIPSEND=";
	u8 *str;
	u16 request_len = strlen(h1) + strlen(para)*2 + strlen(h3) ;
	str = int_to_str(temp, request_len);
	strcat(cmd, str);					//组合长度
	strcat(cmd, "\r\n");

	if(!wifi_send_and_wait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))	//发送发送命令，等待回应
		return FAIL;
	Usart2SendString(h1);
	Usart2SendString(para);
	Usart2SendString(h3);
	if(!wifi_send_and_wait(para, "SEND OK", WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	debug_vip("SEND OK");
	return SUCCESS;
}
/*********************************************************************************
设置esp8266 为服务器模式，发个wifi热点，连接后可以进行本地控制和设置
连接流程:设置AP模式-设置ssid和密码-重启-设置多链接-开启服务器-查看ip
*******************************************************************************/
u8 WIFI_SetServerMode(void)
{
	//设置为AP模式
	if( !wifi_send_and_wait(CMD_APMode.send, CMD_APMode.match, WIFI_CMD_TIMEOUT_MS))
		return FAIL;
	//设置wifi 热点信息
	if( !wifi_send_and_wait(CMD_SSID.send, CMD_SSID.match, WIFI_CMD_TIMEOUT_MS ))
	{
		return FAIL;
	}
	//重启
	if( !wifi_send_and_wait(CMD_Reset.send, CMD_Reset.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	delay_s(2);		//重启等待
	//设置多连接，服务器模式必须设置为多连接
	if( !wifi_send_and_wait(CMD_Muti.send, CMD_Muti.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	//设置服务器模式和端口
	if( !wifi_send_and_wait(CMD_Server.send, CMD_Server.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	//查看ip
	if( !wifi_send_and_wait(CMD_IP.send, CMD_IP.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	return SUCCESS;
}
/*****************************************************************************
设置ESP8266 客户端 模式命令和答复关键字
连接流程:设置STA模式-重启-连接wifi-开启单链接-建立TCP连接
*******************************************************************************/
u8 WIFI_SetClientMode(void)
{
	//设置为station 模式
	if( !wifi_send_and_wait(CMD_STAMode.send, CMD_STAMode.match, WIFI_CMD_TIMEOUT_MS))
		return FAIL;
	if( !wifi_send_and_wait(CMD_Reset.send, CMD_Reset.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	delay_s(3);
	//连接wifi，注意wifi连接用时较长，所以给了较长超时时间
	if( !wifi_send_and_wait(CMD_Connect_WIFI.send, CMD_Connect_WIFI.match, 20000 ))	
	{
		return FAIL;
	}
	//设置为单连接模式
	if( !wifi_send_and_wait(CMD_Single.send, CMD_Single.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	//服务器连接也给较长时间
	/*
	if( !wifi_send_and_wait(CMD_Connect_Server.send, CMD_Connect_Server.match, 10000 ))
		return FAIL;
		*/
	return SUCCESS;
}

u8 WIFI_ClientConnectServer(void)
{
	if( !wifi_send_and_wait(CMD_Connect_Server.send, CMD_Connect_Server.match, 10000 ))
		return FAIL;
	return SUCCESS;
}
u8 WIFI_ClientDisonnectServer(void)
{
	if( !wifi_send_and_wait(CMD_Disconnect_Server.send, CMD_Disconnect_Server.match, 5000 ))
		return FAIL;
	return SUCCESS;
}
u8 WIFI_SetClientSingleConnect(void)
{
	if( !wifi_send_and_wait(CMD_Single.send, CMD_Single.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
}
