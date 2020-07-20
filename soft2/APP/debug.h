#ifndef _DEBUG_H_
#define _DEBUG_H_
#include "config.h"
#include "usart.h"

#define __DEBUG__
#define DEBUG_PORT USART1

#ifdef __DEBUG__
	#if DEBUG_PORT==USART4
		#define debug_init()			Usart4Init()
		#define debug(x)				Usart4SendString(x)
		#define debug_var(var)			DebugVar(var)
	#else 
		#define debug_init()			Usart1Init()
		#define debug(x)				Usart1SendString(x)
		#define debug_var(var)			DebugVar(var)
	#endif
#else
		#define debug(x)	
		#define debug_var(var)
#endif
	

void Usart2Init(void);		
void Usart2SendString(u8 *str);
void DebugVar(u16 var);

#endif
