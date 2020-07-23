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
#include "http_ap.h"

typedef enum _t_status_
{
	ap=1,
	client
}_t_status;

_t_status Status;

void main(void)
{
	Status = ap;
	bsp();
	while(1)
	{
		switch(Status)
		{
			case ap:
				if(wifi_receive())
				{
					AP_MSG_Handle();
				}
				break;
			case client:
				break;
			default:
				break;
		}
		
	}
}
