#ifndef _WIFI_H_
#define _WIFI_H_

#include "bsp.h"

#define	FAIL	0
#define	SUCCESS	1
#define	WIFI_RBUF	RX2_Buffer
extern u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//Ω” ’ª∫≥Â

void WIFI_Reset(void);
u8 WIFI_Receive(u16 timeout_ms)	;
u8 WIFI_Server_HTTP_Response(u8 *client_id, u8 *content);
u8 WIFI_Client_HTTP_Request(u8 *h1, u8 *para, u8 *h3);
u8 WIFI_SetServerMode(void);
u8 WIFI_SetClientMode(void);
u8 client_send_to_server( u8 *p);
u8 server_send_to_client(u8 *client_id, u8 *p);
u8 WIFI_ClientConnectServer(void);
u8 WIFI_ClientDisonnectServer(void);
u8 WIFI_SetClientSingleConnect(void);

#endif
