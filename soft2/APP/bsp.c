#include "bsp.h"
#include "sys.h"
#include "string.h"
#include "wifi.h"
void bsp(void)
{

	EA=0;			//关中断
	Usart2Init();		//初始化串口2		
	S2_Int_en();		//开串口2中断
	
	debug_init();		//调试口初始化

	IP=0x00;
	IP2=0x00;
	IP2 |=0x01;			//设置串口2 优先级高
	
	
	sys_tick_init();		//时间片模块初始化

	EA=1;				//开中断
	DevicesInit();		//外设初始化

}

