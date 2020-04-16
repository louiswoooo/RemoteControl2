#include "config.h"
#include "usart.h"
#include "timer_config.h"

u8 sys_tick;
u8 second,minute,hour;

void sys_time_init(void)		//10����@22.1184MHz
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

void sys_time_interrupt(void) interrupt TIMER0_VECTOR
{
	if(sys_tick>0)
	{
		sys_tick--;
	}
	else
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
	}
	
	if(COM3.B_RX_EN)
	{
		if(COM3.RX_TimeOut>0) 
		{
			COM3.RX_TimeOut--;
			if(0==COM3.RX_TimeOut)
			{
				if(COM3.RX_Cnt>0)
				{
					COM3.B_RX_EN=0;
					RX3_Buffer[COM3.RX_Cnt]='\0';
					COM3.B_RX_OK=1;
				}
			}
		}
	}
}
