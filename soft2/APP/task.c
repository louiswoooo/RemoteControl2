#include "wifi.h"
#include "http_text.h"
#include "string.h"
#include "debug.h"
#include "devices.h"
#include "eeprom.h"

#define SERVER_IP		"119.3.233.56"
#define SERVER_PORT		"5000"

/***************************************************************************
eap8266��Ϊ����ˣ�������Ϣ��ʽ:
+IPD,id,length:data, id Ϊ���ӵĿͻ���id��lengthΪ���յ����ַ�������,dataΪ����
eap8266��Ϊ����ˣ�������Ϣ��ʽ:
+IPD,length:lengthΪ���յ����ַ�������,dataΪ����
***************************************************************************/
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
#define HTTP_CLIENT_CONTROL_KEYWORD		"control?" 

u8 get_http_para(u8 *http_head, u8*find_para, u8 *res)
{
	u8 head_size;
	u8 *p_para;
	u8 *p1,*p2, *p;
	head_size = strlen(http_head);
	http_head[head_size-1] = '&';	//���һ��Ϊ�ո�Ϊ��׼�������Ϊ&
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
	p = strstr(http_request_head,  HTTP_SERVER_INDEX_KEYWORD);		//������ҳ?
	if(p)
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//������ҳ
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_CONTROL_KEYWORD))	//�������?
	{
		if(DevicesControl(p) == 1)		//��������
			WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//�ɹ����ؿ���ҳ��
		else
			WIFI_Server_HTTP_Response(client_id, "Devices control Fail !!!");		//ʦ��������Ϣ
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_CONFIG_KEYWORD))	//��������ҳ��
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Config);	//����
	}
	else if(p = strstr(http_request_head, HTTP_SERVER_RES_KEYWORD))	//�������ý��ҳ��
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
		WIFI_Server_HTTP_Response(client_id, config_res);	//�������ý��
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
	DeviceGetStatus(device_para);	//��ȡ�豸��Ϣ�ַ���
	 //������������豸״̬http ����
	if(!WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3))
		return;
	if(WIFI_Receive(1000))		//�ȴ���������Ӧ
	{
		SERVER_LIGHT_OFF();
		p = strstr(WIFI_RBUF, HTTP_CLIENT_CONTROL_KEYWORD);	//�յ���Ч������Ϣ?
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

