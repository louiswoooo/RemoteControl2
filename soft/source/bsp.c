#include "bsp.h"
#include "timer_config.h"

/*
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
*/
void bsp(void)
{
	GPIO_InitTypeDef	light_pin_init, switch_gpio_init;
	
	light_pin_init.Mode=GPIO_OUT_PP;
	light_pin_init.Pin=LIGHT1_GPIO_PIN;
	GPIO_Inilize(LIGHT1_GPIO_PORT, &light_pin_init);
	
	light_pin_init.Pin=LIGHT2_GPIO_PIN|LIGHT3_GPIO_PIN|LIGHT4_GPIO_PIN;
	GPIO_Inilize(LIGHT2_GPIO_PORT, &light_pin_init);

	switch_gpio_init.Mode=GPIO_OUT_PP;
	switch_gpio_init.Pin=SWITCH1_GPIO_PIN|SWITCH2_GPIO_PIN|SWITCH3_GPIO_PIN|SWITCH4_GPIO_PIN;
	GPIO_Inilize(SWITCH1_GPIO_PORT,SWITCH1_GPIO_PORT);



	USART2_Config();
	S2_Int_en();
	
	USART4_Config();
	S4_Int_en();

	IP=0x00;
	IP2=0x00;
	IP2 |=0x01;			//设置串口2 优先级高
	
	
	sys_time_init();

	EA=1;

	wifi_reset();
			

}

void sys_sleep(void)
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	PCON |= 0x01;
}


