#ifndef _WIFI_H_
#define _WIFI_H_

#include "bsp.h"

#define	FAIL	0
#define	SUCCESS	1
#define	WIFI_RBUF	RX2_Buffer
extern u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//Ω” ’ª∫≥Â

void wifi_reset(void);
u8 wifi_receive();
u8 WIFI_Set_AP_mode(void);
u8 http_send(u8 *client_id, u8 *content);
u8 send_to_client(u8 *client_id, u8 *p);

#endif
