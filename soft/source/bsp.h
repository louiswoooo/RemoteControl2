#ifndef _BSP_H_
#define _BSP_H_

#include <config.h>
#include "gpio.h"
#include "timer_config.h"
#include <usart.h>
#include "intrins.h"
#include "wifi.h"

#define __DEBUG__
#ifdef __DEBUG__
	#define debug(x)		PrintString4(x)
#else
	#define debug(x)	
#endif
#define debug_var(var)			Debug_Var(var)

#define LIGHT1_GPIO_PORT 	GPIO_P4
#define LIGHT1_GPIO_PIN		GPIO_Pin_5
#define LIGHT2_GPIO_PORT 	GPIO_P2
#define LIGHT2_GPIO_PIN		GPIO_Pin_7
#define LIGHT3_GPIO_PORT 	GPIO_P2
#define LIGHT3_GPIO_PIN		GPIO_Pin_6
#define LIGHT4_GPIO_PORT 	GPIO_P2
#define LIGHT4_GPIO_PIN		GPIO_Pin_5

#define SWITCH1_GPIO_PORT 		GPIO_P2
#define SWITCH1_GPIO_PIN		GPIO_Pin_0
#define SWITCH2_GPIO_PORT 		GPIO_P2
#define SWITCH2_GPIO_PIN		GPIO_Pin_1
#define SWITCH3_GPIO_PORT 		GPIO_P2
#define SWITCH3_GPIO_PIN		GPIO_Pin_2
#define SWITCH4_GPIO_PORT 		GPIO_P2
#define SWITCH4_GPIO_PIN		GPIO_Pin_3

#define LIGHT1_ON	GPIO_SetBit(LIGHT1_GPIO_PORT,LIGHT1_GPIO_PIN)
#define LIGHT1_OFF	GPIO_ClearBit(LIGHT1_GPIO_PORT,LIGHT1_GPIO_PIN)
#define LIGHT2_ON	GPIO_SetBit(LIGHT2_GPIO_PORT,LIGHT2_GPIO_PIN)
#define LIGHT2_OFF	GPIO_ClearBit(LIGHT2_GPIO_PORT,LIGHT2_GPIO_PIN)
#define LIGHT3_ON	GPIO_SetBit(LIGHT3_GPIO_PORT,LIGHT3_GPIO_PIN)
#define LIGHT3_OFF	GPIO_ClearBit(LIGHT3_GPIO_PORT,LIGHT3_GPIO_PIN)
#define LIGHT4_ON	GPIO_SetBit(LIGHT4_GPIO_PORT,LIGHT4_GPIO_PIN)
#define LIGHT4_OFF	GPIO_ClearBit(LIGHT4_GPIO_PORT,LIGHT4_GPIO_PIN)



void bsp(void);
void sys_sleep(void);

void Debug_Var(u16 var);
#endif

