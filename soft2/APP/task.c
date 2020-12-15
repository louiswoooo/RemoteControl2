#include "wifi.h"
#include "http_text.h"
#include "string.h"
#include "debug.h"
#include "devices.h"
#include "eeprom.h"

#define RELOGIN_TIMES	10
u16 HeartBeat_TimeoutCounter, HeartBeat_ErrMsgCounter, HeartBeat_OKCounter;
u16 HeartBeat_ConnectFailCounter, HeartBeat_SendFailCounter;
u8 xdata PreStatus[100];

typedef enum _e_status_
{
	init = 1,
	server_mode,
	client_mode
}_e_status;

extern _e_status Sys_Status;
typedef enum _e_client_status_
{
	unkown=1,
	not_login,
	heart_beat
}_e_client_status;

extern _e_client_status Client_Status;
//重登陆计数器，每次收到心跳则清零，超过10次未收到心跳则重新登录
u8 ReloginTimes;
u16 ReloginCounter;

/***************************************************************************
eap8266作为服务端，接收消息格式:
+IPD,id,length:data, id 为连接的客户端id，length为接收到的字符串长度,data为数据
eap8266作为服务端，接收消息格式:
+IPD,length:length为接收到的字符串长度,data为数据
***************************************************************************/
//server模式和client模式受到消息关键字
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
#define HTTP_CLIENT_SERVER_REPLY_CONTROL		"status=ok" 
#define HTTP_CLIENT_SERVER_REPLY_REGIST_OK		"bregist=ok" 
#define HTTP_CLIENT_SERVER_REPLY_LOGIN_OK		"blogin=ok" 

#define REGIST_PRE	"bregist?"
#define LOGIN_PRE	"blogin?"
#define STATUS_PRE	"status?"

u8 EEPROM_EraseAndWirteString(u16 block_addr, u8 *str)
{
	if(strlen(str) > 511)
		return 0;
	else
	{
		EEPROM_SectorErase(block_addr);		//擦除
		EEPROM_write_n(block_addr, str, strlen(str)+1);	//写入flash
		return 1;
	}
}
u8 get_http_para(u8 *http_head, u8*find_para, u8 *res)
{
	u8 *p_para;
	u8 *p1,*p2, *p;
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

/****************************************************************************************
  * @brief:	web server 模式服务函数，该函在wifi模块进入服务端状态以后，处理wifi
  缓冲区当中收到的字符串，判断连接的web 客户端的所请求的页面，
  然后返回所请求的页面  给客户端
  流程:首先判断是否为http 请求，获取client_id( esp8266可以连接  多个客户端，
  需要判断是哪个客户端连接)，判断页面关键字，处理请求，返回定义页面
  * @param:	
  * @retval:	
*****************************************************************************************/
void task_Server(void)
{
	u8 ssid[32];
	u8 pwd[32];
	u8 ip[16];
	u8 port[6];
	u8 user[20];
	u8 upwd[20];
	u8 http_request_head[200];
	u8 http_request_head_size;


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
	p = strstr(http_request_head,  HTTP_SERVER_INDEX_KEYWORD);		//请求的是首页?
	if(p)
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//发送首页
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_CONTROL_KEYWORD))	//请求控制页?
	{
		if(DevicesControl(p) == 1)		//控制外设
			WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//成功则返回控制页面
		else
			WIFI_Server_HTTP_Response(client_id, "Devices control Fail !!!");		//失败返回信息
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_CONFIG_KEYWORD))	//请求实验板配置页面
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Config);	//发送配置页面
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_RES_KEYWORD))	//请求配置结果页面
	{
		http_request_head_size= strlen(http_request_head);
		http_request_head[http_request_head_size-1] = '&';	//最后一个为空格，为标准化处理改为&
		http_request_head[http_request_head_size] = 0x00;	//add 0
		ssid[0] = '\0';
		pwd[0] = '\0';
		ip[0] = '\0';
		port[0] = '\0';
		user[0] = '\0';
		upwd[0] = '\0';
		get_http_para(http_request_head, "ssid", ssid);
		get_http_para(http_request_head, "pwd", pwd);
		get_http_para(http_request_head, "ip", ip);
		get_http_para(http_request_head, "port", port);
		get_http_para(http_request_head, "user", user);
		get_http_para(http_request_head, "upwd", upwd);
		config_res =  check_http_para(ssid, pwd, ip, port,user, upwd);
		WIFI_Server_HTTP_Response(client_id, config_res);	//发送配置结果
		if(strstr(config_res, "OK"))				//如果参数正确,写入flash
		{
			EEPROM_EraseAndWirteString(EEPROM_FIRST_ADDRESS, http_request_head);		//把配置信息写入第一区
			delay_s(1);
			debug("$$$$$$$$$$$$$$$$$$$$$$$$");
			debug_vip(http_request_head);
			Sys_Status = init;			//转换状态
		}
	}
}

/****************************************************************************************
  * @brief:	本函数在远程服务器进行注册
  * @param:	server, 远程服务器ip
  port, 远程服务器端口
  user, 注册的用户名
  upwd，用户密码
  * @retval:	
*****************************************************************************************/
void client_regist(u8 *server, u8 *port, u8 *user, u8 *upwd)
{
	u8 device_para[100];
	if(!WIFI_ClientConnectServer(server, port))		//连接远程服务器
		return;
	SERVER_LIGHT_ON();		//亮连接灯
	device_para[0] = '\0';
	strcat(device_para, REGIST_PRE);	//按照协议制作注册字符串
	strcat(device_para, "user=");
	strcat(device_para, user);
	strcat(device_para , "&pwd=");
	strcat(device_para, upwd);
	if(!WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3))		//向远程服务器发起请求
		return;
	if(WIFI_Receive(3000))		//等待服务器响应
	{
		if(strstr(WIFI_RBUF, HTTP_CLIENT_SERVER_REPLY_REGIST_OK))		//注册是否成功
		{
			EEPROM_EraseAndWirteString(EEPROM_SECOND_ADDRESS, WIFI_RBUF);		// 返回的注册字符串写入eeprom 2区
			Client_Status = not_login;		//转换状态
		}
		else
		{
			delay_s(10);				//如果失败，等待10s继续请求注册
		}
	}
	SERVER_LIGHT_OFF();		//关服务器连接灯
}

/****************************************************************************************
  * @brief:	本函数在远程服务器进行登录
  * @param:	server, 远程服务器ip
  port, 远程服务器端口
  user, 注册的用户名
  upwd，用户密码
  * @retval:	
*****************************************************************************************/
void client_login(u8 *server, u8 *port, u8 *user, u8 *upwd)
{	u8 device_para[100];
	u8 *p;
	if(!WIFI_ClientConnectServer(server, port))
		return;
	SERVER_LIGHT_ON();
	device_para[0] = '\0';
	strcat(device_para, LOGIN_PRE);		//按照协议制作登录字符串
	strcat(device_para, "user=");
	strcat(device_para, user);
	strcat(device_para , "&pwd=");
	strcat(device_para, upwd);
	strcat(device_para, "&status=");
	DeviceGetStatus(device_para);		//获取设备信息字符串
	 //向服务器发送设备状态http 请求
	if(!WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3))
		return;
	if(WIFI_Receive(3000))		//等待服务器响应
	{
		p = strstr(WIFI_RBUF, HTTP_CLIENT_SERVER_REPLY_LOGIN_OK);	//收到有效控制信息?
		PreStatus[0] = '\0';
		strcpy(PreStatus, p);
		if(p)
		{
			DevicesControl(p);		//控制外设
			Client_Status = heart_beat;	//转换状态
			ReloginTimes =0;			//清零重登录计数器
			HeartBeat_TimeoutCounter = 0;
			HeartBeat_ErrMsgCounter = 0;
			HeartBeat_OKCounter = 0;
			HeartBeat_ConnectFailCounter = 0;
			HeartBeat_SendFailCounter = 0;

			
			ReloginCounter++;
		}
		else
		{
			delay_s(10);
		}
	}
	else
	{
		debug("Server response timeout!\r\n");
		
	}
	SERVER_LIGHT_OFF();	
}

/****************************************************************************************
  * @brief:	本函数向远程服务器发起心跳请求
  * @param:	server, 远程服务器ip
  port, 远程服务器端口
  user, 注册的用户名
  upwd，用户密码
  * @retval:	
*****************************************************************************************/
void client_heart_beat(u8 *server, u8 *port, u8 *user, u8 *upwd)
{
	u8 device_para[100];
	u8 *p;
	u16 heartbeat_totalcounter;
	float res;
	debug("--------------------\r\n");
	if(!WIFI_ClientConnectServer(server, port))		//向服务器连接
	{
		WIFI_ClientConnectClose();
		delay_ms(100);
		if(!WIFI_ClientConnectServer(server, port))
		{
			debug("connect fail\r\n");
			HeartBeat_ConnectFailCounter++;
			return;
		}
	}
	SERVER_LIGHT_ON();
	device_para[0] = '\0';
	strcat(device_para, STATUS_PRE);		//心跳字符串
	strcat(device_para, "user=");
	strcat(device_para, user);
	strcat(device_para , "&pwd=");
	strcat(device_para, upwd);
	strcat(device_para, "&status=");
	DeviceGetStatus(device_para);	//获取设备信息字符串
	 //向服务器发送设备状态http 请求
	if(!WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3))
	{
		debug("request fail\r\n");
		HeartBeat_SendFailCounter++;
		return;
	}
	if(WIFI_Receive(3000))		//等待服务器响应
	{
		p = strstr(WIFI_RBUF, HTTP_CLIENT_SERVER_REPLY_CONTROL);	//收到有效控制信息?
		if(p)
		{
			ReloginTimes = 0;
			HeartBeat_OKCounter++;
			DevicesControl(p);
			if(!strstr(p, PreStatus))
			{
				PreStatus[0] = '\0';
				strcpy(PreStatus, p);
				if(!WIFI_ClientConnectServer(server, port))
				{
					WIFI_ClientConnectClose();
					delay_ms(100);
					if(!WIFI_ClientConnectServer(server, port))
					{
						debug("connect fail\r\n");
						HeartBeat_ConnectFailCounter++;
						return;
					}
				}
				device_para[0] = '\0';
				strcat(device_para, STATUS_PRE);		//心跳字符串
				strcat(device_para, "user=");
				strcat(device_para, user);
				strcat(device_para , "&pwd=");
				strcat(device_para, upwd);
				strcat(device_para, "&status=");
				DeviceGetStatus(device_para);	//获取设备信息字符串
				 //向服务器发送设备状态http 请求
				if(!WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3))
				{
					debug("request fail\r\n");
					HeartBeat_SendFailCounter++;
					return;
				}
				if(WIFI_Receive(3000))		//等待服务器响应
				{
					debug("CTRL Report!\r\n");

				}
			}
		}
		else
		{
			debug("recv waste\r\n");
			ReloginTimes++;
			HeartBeat_ErrMsgCounter ++;
			if(ReloginTimes > 10)		//超时10次未收到回复
			{
				Client_Status = not_login;
			}
		}
	}
	else
	{
		debug("recv timeout------------->>>>>>>>\r\n");
		HeartBeat_TimeoutCounter++;
		ReloginTimes++;
		if(ReloginTimes > 10)		//超时10次未收到回复
		{
			Client_Status = not_login;
		}
	}
	SERVER_LIGHT_OFF();	
	heartbeat_totalcounter = HeartBeat_OKCounter + HeartBeat_ErrMsgCounter + HeartBeat_TimeoutCounter;
	debug("#############ReloginCounter:");
	debug_var(ReloginCounter);
	debug("\r\n");
	debug("OK	ErrMsg	Timeout	%	%	%\r\n");
	debug_var(HeartBeat_OKCounter);
	debug("	");
	debug_var(HeartBeat_ErrMsgCounter);
	debug("	");
	debug_var(HeartBeat_TimeoutCounter);
	debug("	");
	res = ((float)HeartBeat_OKCounter/(float)heartbeat_totalcounter)*100.0;
	debug_var((u16)res);
	debug("%	");
	res = ((float)HeartBeat_ErrMsgCounter/(float)heartbeat_totalcounter)*100.0;
	debug_var((u16)res);
	debug("%	");
	res = ((float)HeartBeat_TimeoutCounter/(float)heartbeat_totalcounter)*100.0;
	debug_var((u16)res);
	debug("%\r\n");
	debug("ConnectFail:");
	debug_var(HeartBeat_ConnectFailCounter);
	debug("	SendFail:");
	debug_var(HeartBeat_SendFailCounter);
	debug("\r\n");
}

/****************************************************************************************
  * @brief:	web client 模式服务函数，client 模式分为3个子状态，分别为unknown ，
  首先读取eeprom在远程服务器的注册参数如果未注册，则进行注册，注册后
  进入not_login 状态，实验板在远程服务器进行注册，注册成功以后进入heart_beat
  状态
  * @param:	server, 远程服务器ip
  port, 远程服务器端口
  user, 注册的用户名
  upwd，用户密码
  * @retval:	
*****************************************************************************************/
void task_Client(u8 *server, u8 *port, u8 *user, u8 *upwd)
{
	u8 eeprom_buf[200];
	switch(Client_Status)
	{
		case unkown:			//未知状态
			EEPROM_read_n(EEPROM_SECOND_ADDRESS, eeprom_buf, 200);		//获取eeprom中的参数
			eeprom_buf[199] = 0x00;
			debug("read eeprom_buf: ");
			debug(eeprom_buf);
			debug("\r\n");
			if(!strstr(eeprom_buf, HTTP_CLIENT_SERVER_REPLY_REGIST_OK))		//如果参数不合法
			{
				client_regist(server, port, user, upwd);		//进行远程注册
			}
			else
			{
				Client_Status = not_login;		//转换not_login状态
			}
			break;
		case not_login:		//not_login状态
			client_login(server, port, user, upwd);		//进行远程登录
			break;
		case heart_beat:		//心跳状态
			client_heart_beat(server, port, user, upwd);	//执行心跳查程序
			break;
		default:
			Client_Status = unkown;
				
	}

}

