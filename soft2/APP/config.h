#ifndef _CONFIG_H_
#define _CONFIG_H_

/*********************************************************/

#define MAIN_Fosc		22118400L	//������ʱ��
//#define MAIN_Fosc		12000000L	//������ʱ��
//#define MAIN_Fosc		11059200L	//������ʱ��
//#define MAIN_Fosc		 5529600L	//������ʱ��
//#define MAIN_Fosc		24000000L	//������ʱ��


/*********************************************************/
#include "stc15fxxxx.h"
#include "intrins.h"



/**************************************************************************/

#define Main_Fosc_KHZ	(MAIN_Fosc / 1000)

/***********************************************************/



void delay_10us(u8 count);
void delay_ms(u8 ms);
void delay_s(u8 ms);

#endif
