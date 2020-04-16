#include "bsp.h"
#include <delay.h>
#include "string.h"
/*
void debug_time(void)
{
	debug_var((u16)hour);
	debug(":");
	debug_var((u16)minute);
	debug(":");
	debug_var((u16)second);
	debug("\r\n");
	
}
*/
void main(void)
{
	u8 i;
	bsp();
	
	memset(RX2_Buffer, 0, sizeof(RX2_Buffer));
	
	delay_s(5);
	debug("delay finish\r\n");
	WIFI_SendAndWait("AT\r\n", "OK", 5);
	debug("match!\r\n");
	while(1)
	{
		
	
	}
}
