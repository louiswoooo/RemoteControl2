#include "wifi.h"
#include "http_text.h"
#include "string.h"
#include "debug.h"
#include "devices.h"
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
//客户端判断收到控制信息关键字
#define HTTP_CLIENT_CONTROL_KEYWORD		"control?" 



void task_Server(void)
{
	u8 *p, *temp;
	u8 client_id[2];
	debug(">>>>>>>>>>>>>>>>>>>>task_Server\r\n");
	//浏览器发送的请求信息会很多，为方便处理
	//截取第一行到HTTP之前的请求有效信息
	temp = strstr(WIFI_RBUF, "HTTP");
	if(temp)
	{
		*temp = 0x00;
		p = strstr(WIFI_RBUF, SERVER_MSG_KEYWORD);		//获取链接的client_id
		if(p)
		{
			p = p + sizeof(SERVER_MSG_KEYWORD)-1;
			debug_vip(p);
			client_id[0] = *p;			//
			client_id[1] = '\0';
			p = strstr(WIFI_RBUF,  HTTP_SERVER_INDEX_KEYWORD);		//请求首页?
			if(p)
			{
				WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//发送首页
			}
			else
			{
				p = strstr(WIFI_RBUF, HTTP_SERVER_CONTROL_KEYWORD);	//请求控制?
				if(p)
				{
					if(DevicesControl(p) == 1)		//控制外设
						WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//成功返回控制页面
					else
						WIFI_Server_HTTP_Response(client_id, "Devices control Fail !!!");		//师范返回信息
				}
			}
		}
	}
}

void task_Client(void)
{
	u8 device_para[100];
	u8 *p;
	 //向服务器发送设备状态http 请求
	if(WIFI_ClientConnectServer())
	{
		device_para[0] = 0;
		DeviceGetStatus(device_para);	//获取设备信息字符串
		WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3);
		if(WIFI_Receive(1000))		//等待服务器响应
		{
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
	}
}

