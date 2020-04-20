#include "bsp.h"
#include <delay.h>
#include "string.h"
#include "devices.h"
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
	
	wifi_reset();
	WIFI_Set_AP_mode();
	debug("server create!!!\r\n");
	while(1)
	{
		DevicesControl();
		
	}
}
