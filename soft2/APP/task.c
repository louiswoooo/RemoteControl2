#include "wifi.h"
#include "http_text.h"
#include "string.h"
#include "debug.h"
#include "devices.h"
#include "eeprom.h"

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

u8 get_http_para(u8 *http_head, u8*find_para, u8 *res)
{
	u8 head_size;
	u8 *p_para;
	u8 *p1,*p2, *p;
	head_size = strlen(http_head);
	http_head[head_size-1] = '&';	//最后一个为空格，为标准化处理改为&
	if(!(p_para = strstr(http_head, find_para)))
		return FAIL;
	p1 = p_para + strlen(find_para)+1;
	if(!(p2 = strstr(p1, "&")))
		return FAIL;
	p = res;
	while(p1<p2)
	{
		*p = *p1;
		p1++;
		p++;
	}
	*p = '\0';
	debug_vip(find_para);
	debug_vip(res);
	return SUCCESS;
}

u8 *check_http_para(u8 *ssid, u8 *pwd, u8 * server, u8 *port, u8 *user, u8 *upwd)
{
	if(strlen(ssid) == 0)
		return "WiFi ssid is Empty!";
	if(strlen(pwd) < 8)
	{
		return "WiFi password is less than 8 bytes!";
	}
	if(strlen(server) == 0)
	{
		return "Server ip is empty!"; 
	}
	if(strlen(port) == 0)
	{
		return "Server port is empty!"; 
	}
	if(strlen(user) == 0)
	{
		return "User is empty!"; 
	}
	if(strlen(upwd) == 0)
	{
		return "User password is empty!"; 
	}
	return "Config OK";

}

void task_Server(void)
{
	u8 ssid[32];
	u8 pwd[32];
	u8 server[16];
	u8 port[6];
	u8 user[20];
	u8 upwd[20];
	u8 http_request_head[200];

	u8 *config_res;
	
	u8 *p, *temp;
	u8 client_id[2];
	temp = strstr(WIFI_RBUF, "HTTP");	//	判断是否为http 请求
	if(!temp)
		return;
	//浏览器发送的请求信息会很多，为方便处理
	//截取第一行到HTTP之前的请求有效信息
	*temp = 0x00;
	http_request_head[0] = '\0';
	strcpy(http_request_head, WIFI_RBUF);
	p = strstr(http_request_head, SERVER_MSG_KEYWORD);		//获取链接的client_id
	if(!p)
		return;
	p = p + sizeof(SERVER_MSG_KEYWORD)-1;
	client_id[0] = *p;			//
	client_id[1] = '\0';
	p = strstr(http_request_head,  HTTP_SERVER_INDEX_KEYWORD);		//请求首页?
	if(p)
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//发送首页
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_CONTROL_KEYWORD))	//请求控制?
	{
		if(DevicesControl(p) == 1)		//控制外设
			WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//成功返回控制页面
		else
			WIFI_Server_HTTP_Response(client_id, "Devices control Fail !!!");		//师范返回信息
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_CONFIG_KEYWORD))	//请求配置页面
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Config);	//发送
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_RES_KEYWORD))	//请求配置结果页面
	{
		ssid[0] = '\0';
		pwd[0] = '\0';
		server[0] = '\0';
		port[0] = '\0';
		user[0] = '\0';
		upwd[0] = '\0';
		get_http_para(http_request_head, "ssid", ssid);
		get_http_para(http_request_head, "pwd", pwd);
		get_http_para(http_request_head, "server", server);
		get_http_para(http_request_head, "port", port);
		get_http_para(http_request_head, "user", user);
		get_http_para(http_request_head, "upwd", upwd);
		config_res =  check_http_para(ssid, pwd, server, port,user, upwd);
		WIFI_Server_HTTP_Response(client_id, config_res);	//发送配置结果
		if(strstr(config_res, "OK"))		
		{
			EEPROM_SectorErase(EEPROM_START_ADDRESS);
			EEPROM_write_n(EEPROM_START_ADDRESS, http_request_head, strlen(http_request_head));
		}
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

