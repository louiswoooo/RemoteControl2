#ifndef _DEBUG_H_
#define _DEBUG_H_
#include "config.h"

#define __DEBUG__
#ifdef __DEBUG__
	#define debug_init()			Usart4Init()
	#define debug(x)				Usart4SendString(x)
	#define debug_var(var)			DebugVar(var)
#else
	#define debug(x)	
	#define debug_var(var)
#endif

void Usart2Init(void);		
void Usart2SendString(u8 *str);
void DebugVar(u16 var);

#endif