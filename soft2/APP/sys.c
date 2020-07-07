/*****************************************************************************
  * @file:    	sys_tick.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-22
  * @brief:	ϵͳʱ��Ƭģ�飬Ϊ����ϵͳ�ṩʱ��Ƭ��
	����Ŀʹ��22.1184Mhz�ⲿ���񣬶�ʱ��t0ÿ��10ms����һ���жϣ�
	�жϺ������ÿ������ʱ��Ƭ������-1���Ӷ������������
	ÿ�뾭��100��ʱ��Ƭ����Щʱ��Ƭ�����Թ�������е���
	���⻹�ṩ��һ��ʱ�����񹩵���
*******************************************************************************/
#include "config.h"
#include "debug.h"
#include "usart.h"


#define TICK_PER_SECOND	100			//����ÿ���ʱ��Ƭsys_tick��
u8 sys_tick;										//ʱ��Ƭ������
u8 second,minute,hour;					//ʱ���������

/****************************************************************************************
  * @brief:	��ʼ��ʱ��Ƭ��ʱ��t0����ʼ��ʱ��������� 
  * @param:	none
  * @retval:	none
*****************************************************************************************/
void sys_tick_init(void)		//10����@22.1184MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0xB8;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ

	ET0=1;

	sys_tick=TICK_PER_SECOND;
	second=0;
	minute=0;
	hour=0;
}

/****************************************************************************************
  * @brief:	ʱ��Ƭ��ʱ���жϷ������ÿ�����������-1
  			��ÿ�������ʱ��-1
  			�Դ��ڵĽ��ճ�ʱʱ��-1
  * @param:	none
  * @retval:	none
*****************************************************************************************/
void sys_tick_int(void) interrupt TIMER0_VECTOR using 1
{
	if(sys_tick>0)
	{
		sys_tick--;
	}
	
	if(COM2.B_RX_EN)			//��������ڽ���
	{
		if(COM2.RX_TimeOut>0) 		//��ʱ������0
		{
			COM2.RX_TimeOut--;		//��ʱ��-1
			if(0==COM2.RX_TimeOut)	//�����ʱ������ʱ���
			{
				COM2.B_RX_EN=0;	//��ֹ����
				if(COM2.RX_Cnt>0)	//����յ�����
				{
					RX2_Buffer[COM2.RX_Cnt]='\0';	//���0
					COM2.B_RX_OK=1;				//���ÿ������ϱ�־
				}
			}
		}
	}
}
/****************************************************************************************
  * @brief:	ʱ������ÿ��1s��ӡʱ��
  * @param:	none
  * @retval:	none
*****************************************************************************************/
/*void task0_Clock(void)
{
	if(sys_tick == 0)
	{
		sys_tick=TICK_PER_SECOND;
		second++;
		if(second>=60)
		{
			second=0;
			minute++;
			if(minute>=60)
			{
				minute=0;
				hour++;
			}
		}
		debug("ʱ�䣺");
		debug_var((u16)hour);
		debug(":");
		debug_var((u16)minute);
		debug(":");
		debug_var((u16)second);
		debug("\r\n");
	}
}
//�������
void sys_idle(void)
{
	PCON = 0x01;
}
//����˯��
void sys_sleep(void)
{
	PCON = 0x02;
}

#define DOG_FLAG_MASK			0x80
#define DOG_ENABLE_MASK		0x20
#define	DOG_CLEAR_MASK		0x10
#define	DOG_IDLECOUNT_MASK 0x08
#define DOG_PRESCALER_MASK			0xf8
#define DOG_PRESCALER			6

//���Ź�����
//ʹ�ܣ��忴�Ź������м���
void dog_init(void)
{
	WDT_CONTR =0x00;
	WDT_CONTR = DOG_ENABLE_MASK | DOG_CLEAR_MASK | DOG_IDLECOUNT_MASK;
	WDT_CONTR |= DOG_PRESCALER;
	
}
//�忴�Ź�
void dog_clear(void)
{
	WDT_CONTR |= DOG_CLEAR_MASK;
}
*/