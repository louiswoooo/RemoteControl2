#ifndef _TASK_H_
#define _TASK_H_

#include "config.h"

void task_Server(void);
void task_Client(u8 *server, u8 *port, u8 *user, u8 *upwd);
u8 get_http_para(u8 *http_head, u8*find_para, u8 *res);
u8 *check_http_para(u8 *ssid, u8 *pwd, u8 * server, u8 *port, u8 *user, u8 *upwd);

#endif
