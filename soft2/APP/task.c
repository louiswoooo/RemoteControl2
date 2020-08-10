#include "wifi.h"
#include "http_text.h"
#include "string.h"
#include "debug.h"
#include "devices.h"

#define SERVER_IP		"119.3.233.56"
#define SERVER_PORT		"5000"

/***************************************************************************
eap8266作为服务端，接收消息格式:
+IPD,id,length:data, id 为连接的客户端id，length为接收到的字符串长度,data为数据
eap8266作为服务端，接收消息格式:
+IPD,length:length为接收到的字符串长度,data为数据
***************************************************************************/
#define SERVER_MSG_KEYWORD	"+IPD,"
#define CLIENT_MSG_KEYWORD	"+IPD,"
//服务端判断请求首页关键字
#define HTTP_SERVER_INDEX_KEYWORD		"GET / " 		
//服务端判断请求控制页关键字
#define HTTP_SERVER_CONTROL_KEYWORD		"GET /?" 	
//服务端判断请求控制页关键字
#define HTTP_SERVER_CONFIG_KEYWORD		"GET /config" 	
//服务端判断请求控制页关键字
#define HTTP_SERVER_RES_KEYWORD		"GET /res" 	


//客户端判断收到控制信息关键字
#define HTTP_CLIENT_CONTROL_KEYWORD		"control?" 



void task_Server(void)
{
	u8 *p, *temp;
	u8 client_id[2];
	temp = strstr(WIFI_RBUF, "HTTP");	//	判断是否为http 请求
	if(!temp)
		return;
	//浏览器发送的请求信息会很多，为方便处理
	//截取第一行到HTTP之前的请求有效信息
	*temp = 0x00;
	p = strstr(WIFI_RBUF, SERVER_MSG_KEYWORD);		//获取链接的client_id
	if(!p)
		return;
	p = p + sizeof(SERVER_MSG_KEYWORD)-1;
	client_id[0] = *p;			//
	client_id[1] = '\0';
	p = strstr(WIFI_RBUF,  HTTP_SERVER_INDEX_KEYWORD);		//请求首页?
	if(p)
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//发送首页
	}
	else if(p = strstr(WIFI_RBUF, HTTP_SERVER_CONTROL_KEYWORD))	//请求控制?
	{
		if(DevicesControl(p) == 1)		//控制外设
			WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//成功返回控制页面
		else
			WIFI_Server_HTTP_Response(client_id, "Devices control Fail !!!");		//师范返回信息
	}
	else if(p = strstr(WIFI_RBUF, HTTP_SERVER_CONFIG_KEYWORD))	//请求配置页面
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Config);	//发送
	}
	else if(p = strstr(WIFI_RBUF, HTTP_SERVER_RES_KEYWORD))	//请求配置结果页面
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Res);	//发送
	}
}

void task_Client(void)
{
	u8 device_para[100];
	u8 *p;
	if(!WIFI_ClientConnectServer(SERVER_IP, SERVER_PORT))
		return;
	SERVER_LIGHT_ON();
	device_para[0] = 0;
	DeviceGetStatus(device_para);	//获取设备信息字符串
	 //向服务器发送设备状态http 请求
	if(!WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3))
		return;
	if(WIFI_Receive(1000))		//等待服务器响应
	{
		SERVER_LIGHT_OFF();
		p = strstr(WIFI_RBUF, HTTP_CLIENT_CONTROL_KEYWORD);	//收到有效控制信息?
		if(p)
		{
			DevicesControl(p);
		}
	}
	else
	{
		debug("Server response timeout!\r\n");
		
	}
	SERVER_LIGHT_OFF();
}

