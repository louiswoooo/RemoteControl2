/*****************************************************************************
  * @file:    	main.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-6
  * @brief:	主程序，程序使用状态机方式执行，主状态Sys_Status，
  实验板启动时为init 状态，通过读取eeprom 实验板基本参数判断是否
  已经设置了远程服务器连接，如果未设置，则自己作为web 服务端，
  进入服务端任务，如果设置了，则作为web 客户端，进入客户端任务
	while(1),执行各个任务任务函数在执行的时候需要控制好自己的
	时间计数器，控制好任务的执行时间，不要超时执行
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
//主状态变量
typedef enum _e_status_
{
	init = 1,			//初始化
	server_mode,	//服务器模式
	client_mode		//客户端模式
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
	Sys_Status = init;				//设置主状态为初始化
	Client_Status = unkown;		//分状态为未知
	SystemClear = 0;
	//IAP_CONTR &= (~0x20);
	debug("\r\n\r\nsys start!!!\r\n");
	EEPROM_read_n(EEPROM_FIRST_ADDRESS, eeprom_buf, 200);	//获取实验板基本信息
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
	if(!strstr(config_res,"OK"))		//如果不合法则设置为服务端模式
	{
		memset(RX2_Buffer, 0, sizeof(RX2_Buffer));		//清空接收缓冲区
		while(1)
		{
			debug("wifi server reset\r\n");
			WIFI_Reset();				//硬件重启wifi 模块
			if(WIFI_SetServerMode())		//设置wifi模块为服务器(wifi 热点，让外部设备连接)模式
			{
				debug("wifi set server sucess\r\n");
				break;
			}
		}
		Sys_Status = server_mode;		
	}
	else			//否则为客户端模式
	{
		memset(RX2_Buffer, 0, sizeof(RX2_Buffer));
		while(1)
		{
			debug("wifi client reset\r\n");
			WIFI_Reset();
			if(WIFI_SetClientMode(ssid, pwd))	//设置wifi模块为客户端( 连接外部wifi )模式
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
				if(WIFI_Receive(50))			//等待客户端发送http 请求
				{
					debug(WIFI_RBUF);
					task_Server();			//执行server 模式任务
				}
				break;
			case client_mode:
				if(sys_tick == 0)		//每秒执行一次心跳
				{
					
					temp0 = tick_counter;
					task_Client(ip, port, user, upwd);	//执行client 模式任务
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
