#ifndef _WIFI_H_
#define _WIFI_H_

#include "bsp.h"

#define 	wifi_send(x)	PrintString3(x)

void wifi_reset(void);
u8 wifi_receive(u16 timeout_tick);

#endif
