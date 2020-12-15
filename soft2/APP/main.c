/*****************************************************************************
  * @file:    	main.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-6
  * @brief:	�����򣬳���ʹ��״̬����ʽִ�У���״̬Sys_Status��
  ʵ�������ʱΪinit ״̬��ͨ����ȡeeprom ʵ�����������ж��Ƿ�
  �Ѿ�������Զ�̷��������ӣ����δ���ã����Լ���Ϊweb ����ˣ�
  ��������������������ˣ�����Ϊweb �ͻ��ˣ�����ͻ�������
	while(1),ִ�и���������������ִ�е�ʱ����Ҫ���ƺ��Լ���
	ʱ������������ƺ������ִ��ʱ�䣬��Ҫ��ʱִ��
*******************************************************************************/
#include "config.h"
#include "sys.h"
#include "debug.h"
#include "bsp.h"
#include "task.h"
#include "eeprom.h"
#include "string.h"
#include "eeprom.h"

#define RESET_KEYWORD	"reset"

#define SERVER_IP "192.168.1.111"

extern u8 sys_tick;
u8 SystemClear;
//��״̬����
typedef enum _e_status_
{
	init = 1,			//��ʼ��
	server_mode,	//������ģʽ
	client_mode		//�ͻ���ģʽ
}_e_status;

_e_status Sys_Status;

typedef enum _e_client_status_
{
	unkown=1,
	not_login,
	heart_beat
}_e_client_status;

_e_client_status Client_Status;

extern ReloginCounter;

void get_ip(u8 *ip)
{
	*ip = '\0';
	strcpy(ip, SERVER_IP);
}

extern u16 tick_counter;
void main(void)
{
	u16 temp0;
	u8 ssid[32];
	u8 pwd[32];
	u8 ip[16];
	u8 port[6];
	u8 user[20];
	u8 upwd[20];
	u8 *config_res;
	u8 eeprom_buf[200];
	bsp();
	Sys_Status = init;				//������״̬Ϊ��ʼ��
	Client_Status = unkown;		//��״̬Ϊδ֪
	SystemClear = 0;
	//IAP_CONTR &= (~0x20);
	debug("\r\n\r\nsys start!!!\r\n");
	EEPROM_read_n(EEPROM_FIRST_ADDRESS, eeprom_buf, 200);	//��ȡʵ��������Ϣ
	eeprom_buf[199] = '\0';
	debug("########################\r\n");
	debug_vip(eeprom_buf);
	ssid[0] = '\0';
	pwd[0] = '\0';
	ip[0] = '\0';
	port[0] = '\0';
	user[0] = '\0';
	upwd[0] = '\0';
	get_http_para(eeprom_buf, "ssid", ssid);
	get_http_para(eeprom_buf, "pwd", pwd);
	//get_ip(ip);
	get_http_para(eeprom_buf, "ip", ip);
	get_http_para(eeprom_buf, "port", port);
	get_http_para(eeprom_buf, "user", user);
	get_http_para(eeprom_buf, "upwd", upwd);
	config_res =  check_http_para(ssid, pwd, ip, port,user, upwd);
	debug("@@@@@@@@@@@@@@");
	debug_vip(config_res);
	if(!strstr(config_res,"OK"))		//������Ϸ�������Ϊ�����ģʽ
	{
		memset(RX2_Buffer, 0, sizeof(RX2_Buffer));		//��ս��ջ�����
		while(1)
		{
			debug("wifi server reset\r\n");
			WIFI_Reset();				//Ӳ������wifi ģ��
			if(WIFI_SetServerMode())		//����wifiģ��Ϊ������(wifi �ȵ㣬���ⲿ�豸����)ģʽ
			{
				debug("wifi set server sucess\r\n");
				break;
			}
		}
		Sys_Status = server_mode;		
	}
	else			//����Ϊ�ͻ���ģʽ
	{
		memset(RX2_Buffer, 0, sizeof(RX2_Buffer));
		while(1)
		{
			debug("wifi client reset\r\n");
			WIFI_Reset();
			if(WIFI_SetClientMode(ssid, pwd))	//����wifiģ��Ϊ�ͻ���( �����ⲿwifi )ģʽ
			{
				debug("wifi set Client sucess\r\n");
				break;
			}
		}		
		Sys_Status = client_mode;
		ReloginCounter = 0;
	}
	dog_init();
	
	while(1)
	{
		switch(Sys_Status)
		{
			case server_mode:
				if(WIFI_Receive(50))			//�ȴ��ͻ��˷���http ����
				{
					debug(WIFI_RBUF);
					task_Server();			//ִ��server ģʽ����
				}
				break;
			case client_mode:
				if(sys_tick == 0)		//ÿ��ִ��һ������
				{
					
					temp0 = tick_counter;
					task_Client(ip, port, user, upwd);	//ִ��client ģʽ����
					debug("Time:");
					debug_time();
					debug("ticks counts:");
					debug_var(tick_counter - temp0);
					debug("\r\n");
				}
				sys_sleep();
				break;
			default:
				break;
		}
		if(COM1.RX_Cnt > 0)
		{
			if(strstr(RX1_Buffer, RESET_KEYWORD))
			{
				debug(RX1_Buffer);
				RX1_Buffer[0] = '\0';
				COM1.RX_Cnt = 0;
				sys_reset();
			}
		}
		if(SystemClear)
		{
			sys_clear();
			sys_reset();
		}
		dog_clear();
	}
}
