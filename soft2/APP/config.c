/*****************************************************************************
  * @file:    	config.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-6
  * @brief:	使用22.1184MHz，系统共用函数模块，目前提供两个函数
*******************************************************************************/

#include "config.h"
void delay_10us(u8 count)		//@22.1184MHz
{
	unsigned char i;

	while(count--)
	{
		_nop_();
		_nop_();
		_nop_();
		i = 26;
		while (--i);
	}
}

void  delay_ms(unsigned char ms)	//@22.1184MHz
{
     unsigned int i;
	 do{
	      i = MAIN_Fosc / 13000;
		  while(--i)	;   //14T per loop
     }while(--ms);
}

void delay_s(u8 second)
{
	u8 i;
	while(second--)
	{
		for(i=4;i>0;i--)
		{
			delay_ms(250);
		}
	}
}

