#include "wifi.h"
#include "http_text.h"
#include "string.h"
#include "debug.h"
#include "devices.h"
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
//�ͻ����ж��յ�������Ϣ�ؼ���
#define HTTP_CLIENT_CONTROL_KEYWORD		"control?" 



void task_Server(void)
{
	u8 *p, *temp;
	u8 client_id[2];
	debug(">>>>>>>>>>>>>>>>>>>>task_Server\r\n");
	//��������͵�������Ϣ��ܶ࣬Ϊ���㴦��
	//��ȡ��һ�е�HTTP֮ǰ��������Ч��Ϣ
	temp = strstr(WIFI_RBUF, "HTTP");
	if(temp)
	{
		*temp = 0x00;
		p = strstr(WIFI_RBUF, SERVER_MSG_KEYWORD);		//��ȡ���ӵ�client_id
		if(p)
		{
			p = p + sizeof(SERVER_MSG_KEYWORD)-1;
			debug_vip(p);
			client_id[0] = *p;			//
			client_id[1] = '\0';
			p = strstr(WIFI_RBUF,  HTTP_SERVER_INDEX_KEYWORD);		//������ҳ?
			if(p)
			{
				WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//������ҳ
			}
			else
			{
				p = strstr(WIFI_RBUF, HTTP_SERVER_CONTROL_KEYWORD);	//�������?
				if(p)
				{
					if(DevicesControl(p) == 1)		//��������
						WIFI_Server_HTTP_Response(client_id, HTTP_Server_Index);	//�ɹ����ؿ���ҳ��
					else
						WIFI_Server_HTTP_Response(client_id, "Devices control Fail !!!");		//ʦ��������Ϣ
				}
			}
		}
	}
}

void task_Client(void)
{
	u8 device_para[100];
	u8 *p;
	 //������������豸״̬http ����
	if(WIFI_ClientConnectServer())
	{
		device_para[0] = 0;
		DeviceGetStatus(device_para);	//��ȡ�豸��Ϣ�ַ���
		WIFI_Client_HTTP_Request(HTTP_Client_Request_Head1, device_para, HTTP_Client_Request_Head3);
		if(WIFI_Receive(1000))		//�ȴ���������Ӧ
		{
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
	}
}
