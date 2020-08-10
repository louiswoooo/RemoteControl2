#include "bsp.h"
#include "sys.h"
#include "string.h"
#include "wifi.h"
void bsp(void)
{

	EA=0;
	Usart2Init();
	S2_Int_en();
	
	debug_init();

	IP=0x00;
	IP2=0x00;
	IP2 |=0x01;			//设置串口2 优先级高
	
	
	sys_tick_init();

	EA=1;
	debug("sys start\r\n");
	DevicesInit();

	memset(RX2_Buffer, 0, sizeof(RX2_Buffer));

	while(1)
	{
		debug("wifi reset\r\n");
		WIFI_Reset();
		if(WIFI_SetClientMode())
		{
			debug("wifi set Client sucess\r\n");
			break;
		}
	}		

}

