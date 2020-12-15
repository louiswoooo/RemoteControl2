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
//�ص�½��������ÿ���յ����������㣬����10��δ�յ����������µ�¼
u8 ReloginTimes;
u16 ReloginCounter;

/***************************************************************************
eap8266��Ϊ����ˣ�������Ϣ��ʽ:
+IPD,id,length:data, id Ϊ���ӵĿͻ���id��lengthΪ���յ����ַ�������,dataΪ����
eap8266��Ϊ����ˣ�������Ϣ��ʽ:
+IPD,length:lengthΪ���յ����ַ�������,dataΪ����
***************************************************************************/
//serverģʽ��clientģʽ�ܵ���Ϣ�ؼ���
#define SERVER_MSG_KEYWORD	"+IPD,"
#define CLIENT_MSG_KEYWORD	"+IPD,"
//������ж�������ҳ�ؼ���
#define HTTP_SERVER_INDEX_KEYWORD		"GET / " 		
//������ж��������ҳ�ؼ���
#define HTTP_SERVER_CONTROL_KEYWORD		"GET /?" 	
//������ж��������ҳ�ؼ���
#define HTTP_SERVER_CONFIG_KEYWORD		"GET /config" 	
//������ж��������ҳ�ؼ���
#define HTTP_SERVER_RES_KEYWORD		"GET /res" 	


//�ͻ����ж��յ�������Ϣ�ؼ���
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
		EEPROM_SectorErase(block_addr);		//����
		EEPROM_write_n(block_addr, str, strlen(str)+1);	//д��flash
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
  * @brief:	web server ģʽ���������ú���wifiģ���������״̬�Ժ󣬴���wifi
  �����������յ����ַ������ж����ӵ�web �ͻ��˵��������ҳ�棬
  Ȼ�󷵻��������ҳ��  ���ͻ���
  ����:�����ж��Ƿ�Ϊhttp ���󣬻�ȡclient_id( esp8266��������  ����ͻ��ˣ�
  ��Ҫ�ж����ĸ��ͻ�������)���ж�ҳ��ؼ��֣��������󣬷��ض���ҳ��
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
	temp = strstr(WIFI_RBUF, "HTTP");	//	�ж��Ƿ�Ϊhttp ����
	if(!temp)
		return;
	//��������͵�������Ϣ��ܶ࣬Ϊ���㴦��
	//��ȡ��һ�е�HTTP֮ǰ��������Ч��Ϣ
	*temp = 0x00;
	http_request_head[0] = '\0';
	strcpy(http_request_head, WIFI_RBUF);
	p = strstr(http_request_head, SERVER_MSG_KEYWORD);		//��ȡ���ӵ�client_id
	if(!p)
		return;
	p = p + sizeof(SERVER_MSG_KEYWORD)-1;
	client_id[0] = *p;			//
	client_id[1] = '\0';
	p = strstr(http_request_head,  HTTP_SERVER_INDEX_KEYWORD);		//���������ҳ?
	if(p)
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//������ҳ
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_CONTROL_KEYWORD))	//�������ҳ?
	{
		if(DevicesControl(p) == 1)		//��������
			WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//�ɹ��򷵻ؿ���ҳ��
		else
			WIFI_Server_HTTP_Response(client_id, "Devices control Fail !!!");		//ʧ�ܷ�����Ϣ
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_CONFIG_KEYWORD))	//����ʵ�������ҳ��
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Config);	//��������ҳ��
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_RES_KEYWORD))	//�������ý��ҳ��
	{
		http_request_head_size= strlen(http_request_head);
		http_request_head[http_request_head_size-1] = '&';	//���һ��Ϊ�ո�Ϊ��׼�������Ϊ&
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
		WIFI_Server_HTTP_Response(client_id, config_res);	//�������ý��
		if(strstr(config_res, "OK"))				//���������ȷ,д��flash
		{
			EEPROM_EraseAndWirteString(EEPROM_FIRST_ADDRESS, http_request_head);		//��������Ϣд���һ��
			delay_s(1);
			debug("$$$$$$$$$$$$$$$$$$$$$$$$");
			debug_vip(http_request_head);
			Sys_Status = init;			//ת��״̬
		}
	}
}

/****************************************************************************************
  * @brief:	��������Զ�̷���������ע��
  * @param:	server, Զ�̷�����ip
  port, Զ�̷������˿�
  user, ע����û���
  upwd���û�����
  * @retval:	
*****************************************************************************************/
void client_regist(u8 *server, u8 *port, u8 *user, u8 *upwd)
{
	u8 device_para[100];
	if(!WIFI_ClientConnectServer(server, port))		//����Զ�̷�����
		return;
	SERVER_LIGHT_ON();		//�����ӵ�
	device_para[0] = '\0';
	strcat(device_para, REGIST_PRE);	//����Э������ע���ַ���
	strcat(device_para, "user=");
	strcat(device_para, user);
	strcat(device_para , "&pwd=");
	strcat(device_para, upwd);
	if(!WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3))		//��Զ�̷�������������
		return;
	if(WIFI_Receive(3000))		//�ȴ���������Ӧ
	{
		if(strstr(WIFI_RBUF, HTTP_CLIENT_SERVER_REPLY_REGIST_OK))		//ע���Ƿ�ɹ�
		{
			EEPROM_EraseAndWirteString(EEPROM_SECOND_ADDRESS, WIFI_RBUF);		// ���ص�ע���ַ���д��eeprom 2��
			Client_Status = not_login;		//ת��״̬
		}
		else
		{
			delay_s(10);				//���ʧ�ܣ��ȴ�10s��������ע��
		}
	}
	SERVER_LIGHT_OFF();		//�ط��������ӵ�
}

/****************************************************************************************
  * @brief:	��������Զ�̷��������е�¼
  * @param:	server, Զ�̷�����ip
  port, Զ�̷������˿�
  user, ע����û���
  upwd���û�����
  * @retval:	
*****************************************************************************************/
void client_login(u8 *server, u8 *port, u8 *user, u8 *upwd)
{	u8 device_para[100];
	u8 *p;
	if(!WIFI_ClientConnectServer(server, port))
		return;
	SERVER_LIGHT_ON();
	device_para[0] = '\0';
	strcat(device_para, LOGIN_PRE);		//����Э��������¼�ַ���
	strcat(device_para, "user=");
	strcat(device_para, user);
	strcat(device_para , "&pwd=");
	strcat(device_para, upwd);
	strcat(device_para, "&status=");
	DeviceGetStatus(device_para);		//��ȡ�豸��Ϣ�ַ���
	 //������������豸״̬http ����
	if(!WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3))
		return;
	if(WIFI_Receive(3000))		//�ȴ���������Ӧ
	{
		p = strstr(WIFI_RBUF, HTTP_CLIENT_SERVER_REPLY_LOGIN_OK);	//�յ���Ч������Ϣ?
		PreStatus[0] = '\0';
		strcpy(PreStatus, p);
		if(p)
		{
			DevicesControl(p);		//��������
			Client_Status = heart_beat;	//ת��״̬
			ReloginTimes =0;			//�����ص�¼������
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
  * @brief:	��������Զ�̷�����������������
  * @param:	server, Զ�̷�����ip
  port, Զ�̷������˿�
  user, ע����û���
  upwd���û�����
  * @retval:	
*****************************************************************************************/
void client_heart_beat(u8 *server, u8 *port, u8 *user, u8 *upwd)
{
	u8 device_para[100];
	u8 *p;
	u16 heartbeat_totalcounter;
	float res;
	debug("--------------------\r\n");
	if(!WIFI_ClientConnectServer(server, port))		//�����������
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
	strcat(device_para, STATUS_PRE);		//�����ַ���
	strcat(device_para, "user=");
	strcat(device_para, user);
	strcat(device_para , "&pwd=");
	strcat(device_para, upwd);
	strcat(device_para, "&status=");
	DeviceGetStatus(device_para);	//��ȡ�豸��Ϣ�ַ���
	 //������������豸״̬http ����
	if(!WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3))
	{
		debug("request fail\r\n");
		HeartBeat_SendFailCounter++;
		return;
	}
	if(WIFI_Receive(3000))		//�ȴ���������Ӧ
	{
		p = strstr(WIFI_RBUF, HTTP_CLIENT_SERVER_REPLY_CONTROL);	//�յ���Ч������Ϣ?
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
				strcat(device_para, STATUS_PRE);		//�����ַ���
				strcat(device_para, "user=");
				strcat(device_para, user);
				strcat(device_para , "&pwd=");
				strcat(device_para, upwd);
				strcat(device_para, "&status=");
				DeviceGetStatus(device_para);	//��ȡ�豸��Ϣ�ַ���
				 //������������豸״̬http ����
				if(!WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3))
				{
					debug("request fail\r\n");
					HeartBeat_SendFailCounter++;
					return;
				}
				if(WIFI_Receive(3000))		//�ȴ���������Ӧ
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
			if(ReloginTimes > 10)		//��ʱ10��δ�յ��ظ�
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
		if(ReloginTimes > 10)		//��ʱ10��δ�յ��ظ�
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
  * @brief:	web client ģʽ��������client ģʽ��Ϊ3����״̬���ֱ�Ϊunknown ��
  ���ȶ�ȡeeprom��Զ�̷�������ע��������δע�ᣬ�����ע�ᣬע���
  ����not_login ״̬��ʵ�����Զ�̷���������ע�ᣬע��ɹ��Ժ����heart_beat
  ״̬
  * @param:	server, Զ�̷�����ip
  port, Զ�̷������˿�
  user, ע����û���
  upwd���û�����
  * @retval:	
*****************************************************************************************/
void task_Client(u8 *server, u8 *port, u8 *user, u8 *upwd)
{
	u8 eeprom_buf[200];
	switch(Client_Status)
	{
		case unkown:			//δ֪״̬
			EEPROM_read_n(EEPROM_SECOND_ADDRESS, eeprom_buf, 200);		//��ȡeeprom�еĲ���
			eeprom_buf[199] = 0x00;
			debug("read eeprom_buf: ");
			debug(eeprom_buf);
			debug("\r\n");
			if(!strstr(eeprom_buf, HTTP_CLIENT_SERVER_REPLY_REGIST_OK))		//����������Ϸ�
			{
				client_regist(server, port, user, upwd);		//����Զ��ע��
			}
			else
			{
				Client_Status = not_login;		//ת��not_login״̬
			}
			break;
		case not_login:		//not_login״̬
			client_login(server, port, user, upwd);		//����Զ�̵�¼
			break;
		case heart_beat:		//����״̬
			client_heart_beat(server, port, user, upwd);	//ִ�����������
			break;
		default:
			Client_Status = unkown;
				
	}

}

