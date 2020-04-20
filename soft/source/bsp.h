#ifndef _BSP_H_
#define _BSP_H_

#include <config.h>
#include "gpio.h"
#include "timer_config.h"
#include <usart.h>
#include "intrins.h"
#include "wifi.h"
#include "delay.h"

#define __DEBUG__
#ifdef __DEBUG__
	#define debug(x)		PrintString4(x)
#else
	#define debug(x)	
#endif
#define debug_var(var)			Debug_Var(var)

void bsp(void);
void sys_sleep(void);

void Debug_Var(u16 var);
#endif

