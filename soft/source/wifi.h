#ifndef _WIFI_H_
#define _WIFI_H_

#include "bsp.h"

#define 	wifi_send(x)	PrintString2(x)

void wifi_reset(void);
u8 wifi_receive();
u8 *WIFI_SendAndWait(u8 *send, u8 *match, u8 try_count);

#endif
