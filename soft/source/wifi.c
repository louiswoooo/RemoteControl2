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

u8 wifi_receive()
{
	COM2.B_RX_EN=1;
	COM2.RX_Cnt=0;
	COM2.B_RX_OK=0;
	COM2.RX_TimeOut=TimeOutSet2;
	while(COM2.RX_TimeOut>0);			//每次串口接收不到字节产生超时，判定为一次块接收完成
	if(COM2.B_RX_OK)
	{
		return COM2.RX_Cnt;
	}
	return NULL;
}

u8 *WIFI_SendAndWait(u8 *send, u8 *match, u8 try_count)
{
	u8 *p;
	u8 i;
	for(i=try_count; i>0; i--)
	{
		wifi_send(send);
		if(wifi_receive())
		{
			if(p=strstr(RX2_Buffer,match))
				return p;
		}
	}
	return NULL;
}
