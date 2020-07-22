#ifndef _HTTP_AP_H_
#define _HTTP_AP_H_

#include "config.h"
#define HTTP_REQUEST_INDEX		"GET / " 
#define HTTP_REQUEST_CONTROL		"GET /?" 

extern u8 code HTTP_H1[];
extern u8 code HTTP_INDEX_CONTENT[];
void AP_MSG_Handle(void);

#endif
