#include "wifi.h"
#include "http_text.h"
#include "string.h"
#include "debug.h"
#include "devices.h"

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



void task_Server(void)
{
	u8 *p, *temp;
	u8 client_id[2];
	temp = strstr(WIFI_RBUF, "HTTP");	//	�ж��Ƿ�Ϊhttp ����
	if(!temp)
		return;
	//��������͵�������Ϣ��ܶ࣬Ϊ���㴦��
	//��ȡ��һ�е�HTTP֮ǰ��������Ч��Ϣ
	*temp = 0x00;
	p = strstr(WIFI_RBUF, SERVER_MSG_KEYWORD);		//��ȡ���ӵ�client_id
	if(!p)
		return;
	p = p + sizeof(SERVER_MSG_KEYWORD)-1;
	client_id[0] = *p;			//
	client_id[1] = '\0';
	p = strstr(WIFI_RBUF,  HTTP_SERVER_INDEX_KEYWORD);		//������ҳ?
	if(p)
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//������ҳ
	}
	else if(p = strstr(WIFI_RBUF, HTTP_SERVER_CONTROL_KEYWORD))	//�������?
	{
		if(DevicesControl(p) == 1)		//��������
			WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//�ɹ����ؿ���ҳ��
		else
			WIFI_Server_HTTP_Response(client_id, "Devices control Fail !!!");		//ʦ��������Ϣ
	}
	else if(p = strstr(WIFI_RBUF, HTTP_SERVER_CONFIG_KEYWORD))	//��������ҳ��
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Config);	//����
	}
	else if(p = strstr(WIFI_RBUF, HTTP_SERVER_RES_KEYWORD))	//�������ý��ҳ��
	{
		WIFI_Server_HTTP_Response(client_id, HTTP_Server_Res);	//����
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

