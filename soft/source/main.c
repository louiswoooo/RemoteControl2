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
	bsp();
	
	memset(RX2_Buffer, 0, sizeof(RX2_Buffer));
	
	//delay_s(10);
	while(1)
	{
		if(wifi_receive())
			debug(RX2_Buffer);
		//WIFI_SendAndWait("AT+CWMODE=2", "OK", 10);
		//debug("response sucess!!!\r\n");
		//delay_s(10);
		
	}
}
