#include "bsp.h"
#include <delay.h>
#include "string.h"

void debug_time(void)
{
	debug_var((u16)hour);
	debug(":");
	debug_var((u16)minute);
	debug(":");
	debug_var((u16)second);
	debug("\r\n");
	
}

void main(void)
{
	bsp();
	
	memset(RX3_Buffer, 0, sizeof(RX3_Buffer));

	while(1)
	{
		if(wifi_receive(100))
		{
			debug("\r\n<<:");
			debug(RX3_Buffer);
		}
		sys_sleep();
		
	}
}
