#ifndef _BSP_H_
#define _BSP_H_

#include <config.h>
#include "gpio.h"
#include <usart.h>
#include "intrins.h"
#include "wifi.h"
#include "devices.h"
#include "debug.h"


void bsp(void);

void Debug_Var(u16 var);
#endif

