#ifndef _WIFI_H_
#define _WIFI_H_

#include "bsp.h"

#define 	wifi_send(x)	PrintString2(x)
#define	FAIL	0
#define	SUCCESS	1

void wifi_reset(void);
u8 wifi_receive();
u8 *WIFI_SendAndWait(u8 *send, u8 *match, u16 timeout_ticks);
u8 WIFI_Set_AP_mode(void);

#endif
