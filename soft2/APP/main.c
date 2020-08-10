/*****************************************************************************
  * @file:    	main.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-6
  * @brief:	�����򣬳�ʼ��ϵͳ����ģ��͸�������
	while(1),ִ�и���������������ִ�е�ʱ����Ҫ���ƺ��Լ���
	ʱ������������ƺ������ִ��ʱ�䣬��Ҫ��ʱִ��
*******************************************************************************/
#include "config.h"
#include "sys.h"
#include "debug.h"
#include "bsp.h"
#include "task.h"

typedef enum _t_status_
{
	server=1,
	client
}_t_status;

_t_status Status;

void main(void)
{
	Status = server;
	bsp();
	while(1)
	{
		switch(Status)
		{
			case server:
				if(WIFI_Receive(50))
				{
					task_Server();
				}
				break;
			case client:
					task_Client();
					delay_s(1);
				break;
			default:
				break;
		}
		
	}
}
