/*****************************************************************************
  * @file:    	sys_tick.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-22
  * @brief:	系统时间片模块，为整个系统提供时间片，
	本项目使用22.1184Mhz外部晶振，定时器t0每隔10ms产生一次中断，
	中断函数会对每个任务时间片计数器-1，从而驱动任务调度
	每秒经历100个时间片，这些时间片都可以供任务进行调度
	另外还提供了一个时间任务供调用
*******************************************************************************/
#include "config.h"
#include "debug.h"
#include "usart.h"


#define TICK_PER_SECOND	100			//定义每秒的时间片sys_tick数
u8 sys_tick;										//时间片计数器
u8 second,minute,hour;					//时钟任务变量

/****************************************************************************************
  * @brief:	初始化时间片定时器t0，初始化时钟任务变量 
  * @param:	none
  * @retval:	none
*****************************************************************************************/
void sys_tick_init(void)		//10毫秒@22.1184MHz
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

/****************************************************************************************
  * @brief:	时间片定时器中断服务程序，每个任务计数器-1
  			对每个任务计时器-1
  			对串口的接收超时时间-1
  * @param:	none
  * @retval:	none
*****************************************************************************************/
void sys_tick_int(void) interrupt TIMER0_VECTOR using 1
{
	if(sys_tick>0)
	{
		sys_tick--;
	}
	
	if(COM2.B_RX_EN)			//如果允许串口接收
	{
		if(COM2.RX_TimeOut>0) 		//计时器大于0
		{
			COM2.RX_TimeOut--;		//计时器-1
			if(0==COM2.RX_TimeOut)	//如果计时器倒计时完毕
			{
				COM2.B_RX_EN=0;	//禁止接收
				if(COM2.RX_Cnt>0)	//如果收到数据
				{
					RX2_Buffer[COM2.RX_Cnt]='\0';	//添加0
					COM2.B_RX_OK=1;				//设置块接收完毕标志
				}
			}
		}
	}
}
/****************************************************************************************
  * @brief:	时钟任务，每隔1s打印时间
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
		debug("时间：");
		debug_var((u16)hour);
		debug(":");
		debug_var((u16)minute);
		debug(":");
		debug_var((u16)second);
		debug("\r\n");
	}
}
//进入空闲
void sys_idle(void)
{
	PCON = 0x01;
}
//进入睡眠
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

//看门狗配置
//使能，清看门狗，空闲计数
void dog_init(void)
{
	WDT_CONTR =0x00;
	WDT_CONTR = DOG_ENABLE_MASK | DOG_CLEAR_MASK | DOG_IDLECOUNT_MASK;
	WDT_CONTR |= DOG_PRESCALER;
	
}
//清看门狗
void dog_clear(void)
{
	WDT_CONTR |= DOG_CLEAR_MASK;
}
*/