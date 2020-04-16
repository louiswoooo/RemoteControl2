#include "config.h"
#include "usart.h"
#include "timer_config.h"

u8 sys_tick;
u8 second,minute,hour;

void sys_time_init(void)		//10毫秒@22.1184MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0xB8;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时

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
