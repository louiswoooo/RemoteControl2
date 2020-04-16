#include "usart.h"
#include "string.h"
#include "gpio.h"
#include "delay.h"
#include "wifi.h"
#define WIFI_GPIO_PORT		GPIO_P4
#define WIFI_GPIO_PIN		GPIO_Pin_2

#define	wifi_reset_pin_low()		GPIO_ClearBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)
#define	wifi_reset_pin_high()		GPIO_SetBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)

void wifi_reset(void)
{
	GPIO_InitTypeDef	wifi_gpio_init;

	wifi_gpio_init.Mode=GPIO_PullUp;
	wifi_gpio_init.Pin=WIFI_GPIO_PIN;
	
	GPIO_Inilize(WIFI_GPIO_PORT, &wifi_gpio_init);

	wifi_reset_pin_low();
	delay_ms(100);
	wifi_reset_pin_high();
}

u8 wifi_receive(u16 timeout_tick)
{
	u16 i;
	for(i = timeout_tick/(COM3.RX_TimeOut); i>0; i--)
	{
		COM3.B_RX_EN=1;
		COM3.RX_Cnt=0;
		COM3.RX_TimeOut=TimeOutSet3;
		while(COM3.RX_TimeOut>0);			//每次串口接收不到字节产生超时，判定为一次块接收完成
		if(COM3.B_RX_OK)
			return COM3.RX_Cnt;
	}
	return NULL;
}

	
u8 *WIFI_SendAndWait(u8 *send,u8 *match,u8 timeout_tick)
{
	u8 *p;
	wifi_send(send);
	if(wifi_receive(timeout_tick))
	{
		if(p=strstr(RX3_Buffer,match))
			return p;
	}
	return NULL;
}
