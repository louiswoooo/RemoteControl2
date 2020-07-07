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
#include "http_ap.h"
#include "string.h"

typedef enum _t_status_
{
	ap=1,
	client
}_t_status;

_t_status Status;

void main(void)
{
	u8 *p;
	Status = 1;
		
	bsp();
	while(1)
	{
		switch(Status)
		{
			case ap:
				if(wifi_receive())
				{
					debug(WIFI_RBUF);
					p = strstr(WIFI_RBUF, PORTAL_KEYWORD);
					wifi_send(PORTAL_PAGE);
				}
				break;
			case client:
				break;
			default:
				break;
		}
		
	}
}
