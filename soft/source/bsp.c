#include "bsp.h"
#include "timer_config.h"

void Debug_Var(u16 var)
{
	u8 num[6];
	num[0]=var/10000+0x30;
	num[1]=var%10000/1000+0x30;
	num[2]=var%1000/100+0x30;
	num[3]=var%100/10+0x30;
	num[4]=var%10+0x30;
	num[5]='\0';
	debug(num);
}
void bsp(void)
{


	USART2_Config();
	S2_Int_en();
	
	USART4_Config();
	S4_Int_en();

	IP=0x00;
	IP2=0x00;
	IP2 |=0x01;			//设置串口2 优先级高
	
	
	sys_time_init();

	EA=1;

	DevicesInit();

	//wifi_reset();
			

}

void sys_sleep(void)
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	PCON |= 0x01;
}


