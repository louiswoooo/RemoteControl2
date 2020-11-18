/*****************************************************************************
  * @file:    	usart.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-22
  * @brief:	´®¿Ú2µÄÇý¶¯³ÌÐò£¬´®¿Ú2ÉèÖÃÎªwifiÄ£¿éµÄ¿ØÖÆ¶Ë¿Ú£¬´®¿Ú²ÉÓÃ¿é½ÓÊÕµÄ½ÓÊÕ»úÖÆÎª:
  			Ê¹ÄÜ´®¿Ú½ÓÊÕ£¬´Ó½ÓÊÕµ½µÚÒ»¸ö´®¿Ú×Ö·û¿ªÊ¼µ¹¼ÆÊ±£¬Èç¹ûÊÕµ½ÐÂ×Ö·ûÔòÖØÐÂ¿ªÊ¼µ¹¼ÆÊ±£¬
  			Èç¹û¼ÆÊ±³¬Ê±£¬ÔòÅÐ¶Ï½ÓÊÕµ½ÁËÊý¾Ý¿é£¬ÓÉÓÚÈ«²¿²ÉÓÃ×Ö·û´®ÃüÁîµÄÍ¨Ñ¶·½Ê½£¬
  			×îºóÔÚ½ÓÊÕµ½µÄ×Ö·û´®ºóÃæ¼Ó0£¬´Ëºó½ûÖ¹½ÓÊÕ£¬µÈ´ýÈÎÎñ´¦Àí¸ÃÃüÁîÍê±Ïºó£¬ÈÎÎñ¿ÉÒÔ
  			ÖØÐÂÆô¶¯½ÓÊÕ
  			´®¿Ú4µÄÇý¶¯³ÌÐò£¬³ö¿Ú4µÄ·¢ËÍ³ÌÐò²ÉÓÃÁË»º³åÇøÖÐ¶Ï·¢ËÍµÄ·½Ê½£¬·¢ËÍ³ÌÐòÏÈ°ÑÊý¾ÝËÍµ½»º³åÇø£¬
  			È»ºóÆô¶¯´¥·¢ÖÐ¶Ï°´ÕÕ·¢ËÍ¶ÁÖ¸Õë½øÐÐ·¢ËÍ£¬Èç¹û¶ÁÖ¸ÕëµÈÓÚÐ´Ö¸Õë£¬Ôò»º³åÇøÊý¾Ý·¢ËÍ½áÊø
*******************************************************************************/
#include "USART.h"

COMx_Define COM2;
u8	xdata TX2_Buffer[COM_TX2_Lenth];	//·¢ËÍ»º³å
u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//½ÓÊÕ»º³å

#if DEBUG_PORT==USART4
COMx_Define COM4;
u8	xdata TX4_Buffer[COM_TX4_Lenth];	//·¢ËÍ»º³å
u8 	xdata RX4_Buffer[COM_RX4_Lenth];	//½ÓÊÕ»º³å
#else
COMx_Define COM1;
u8	xdata TX1_Buffer[COM_TX1_Lenth];	//·¢ËÍ»º³å
u8 	xdata RX1_Buffer[COM_RX1_Lenth];	//½ÓÊÕ»º³å
#endif


//´®¿Ú2³õÊ¼»¯
void Usart2Init(void)		//115200bps@22.1184MHz
{
	S2CON = 0x50;		//8Î»Êý¾Ý,¿É±ä²¨ÌØÂÊ
	AUXR |= 0x04;		//¶¨Ê±Æ÷2Ê±ÖÓÎªFosc,¼´1T
	T2L = 0xD0;		//Éè¶¨¶¨Ê±³õÖµ
	T2H = 0xFF;		//Éè¶¨¶¨Ê±³õÖµ
	AUXR |= 0x10;		//Æô¶¯¶¨Ê±Æ÷2
	
	COM2.id = 2;
	COM2.TX_read    = 0;
	COM2.TX_write   = 0;
	COM2.B_TX_busy  = 0;
	COM2.RX_Cnt     = 0;
	COM2.RX_TimeOut = 0;
	COM2.B_RX_EN    = 0;
	COM2.B_RX_OK    = 0;
}


/****************************************************************************************
  * @brief:	´®¿Ú2×Ö·û·¢ËÍ³ÌÐò£¬ÏÈ°Ñ×Ö·û´æÈë»º³åÇø£¬
  			È»ºóÅÐ¶ÏÊÇ·ñ´®¿ÚÕýÔÚ·¢ËÍ£¬Èç¹ûÃ»ÓÐ£¬Ôò´¥·¢´®¿Ú·¢ËÍ
  * @param:	dat
  * @retval:	none
*****************************************************************************************/
void TX2_write2buff(u8 dat)
{
	S2BUF = dat;
	while(!TI2);
	CLR_TI2();
}
/****************************************************************************************
  * @brief:	´®¿Ú2×Ö·û´®·¢ËÍ³ÌÐò£¬Óöµ½0½áÊø
  * @param:	puts
  * @retval:	none
*****************************************************************************************/
void Usart2SendString(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX2_write2buff(*puts); 	//Óöµ½Í£Ö¹·û0½áÊø
}

/****************************************************************************************
  * @brief:	´®¿Ú2ÖÐ¶Ï³ÌÐò£¬¸ù¾Ý½ÓÊÕ×´Ì¬£¬½ÓÊÕµ½µÄ×Ö·û·ÅÈë»º³åÇø
  			²¢ÇÒÖØÐÂÉèÖÃ½ÓÊÕ¼ÆÊ±Æ÷
  			·¢ËÍÖÐ¶Ï¸ù¾Ý¶ÁÐ´Ö¸ÕëÅÐ¶ÏÊÇ·ñ·¢ËÍÍê³É£¬Èç¹ûÍê³ÉÇåÃ¦±êÖ¾
  * @param:	none
  * @retval:	none
*****************************************************************************************/
void UART2_int (void) interrupt UART2_VECTOR		using 2
{
	if(RI2)
	{
		CLR_RI2();
		if(COM2.B_RX_EN == 1)		//Èç¹ûÔÊÐí½ÓÊÕ
		{
			if(COM2.RX_Cnt >= COM_RX2_Lenth)	COM2.RX_Cnt = 0;	//»º³åÇøÒç³öå£¿
			RX2_Buffer[COM2.RX_Cnt++] = S2BUF;					//ÊÕµ½µÄ×Ö·û·ÅÈë»º³åÇø
			COM2.RX_TimeOut = TimeOutSet2;						//ÖØÐÂÉèÖÃ³¬Ê±¼ÆÊ±Æ÷
		}
	}
}

#if DEBUG_PORT == USART4

//´®¿Ú4³õÊ¼»¯
void Usart4Init(void)		//115200bps@22.1184MHz
{
	S4CON = 0x10;		//8???,?????
	S4CON |= 0x40;		//??4?????4???????
	T4T3M |= 0x20;		//???4???Fosc,?1T
	T4L = 0xD0;		//??????
	T4H = 0xFF;		//??????
	T4T3M |= 0x80;		//?????4

	COM4.id = 4;
	COM4.TX_read    = 0;
	COM4.TX_write   = 0;
	COM4.B_TX_busy  = 0;
	COM4.RX_Cnt     = 0;
	COM4.RX_TimeOut = 0;
	COM4.B_RX_EN    = 0;
	COM4.B_RX_OK    = 0;

	S4_Int_en();


}

void TX4_write2buff(u8 dat)	//Ð´Èë·¢ËÍ»º³å£¬Ö¸Õë+1
{
	TX4_Buffer[COM4.TX_write] = dat;	//×°·¢ËÍ»º³å
	if(++COM4.TX_write >= COM_TX4_Lenth)	COM4.TX_write = 0;

	if(COM4.B_TX_busy == 0)		//¿ÕÏÐ
	{  
		COM4.B_TX_busy = 1;		//±êÖ¾Ã¦
		SET_TI4();				//´¥·¢·¢ËÍÖÐ¶Ï
	}
}

void Usart4SendString(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX4_write2buff(*puts); 	//Óöµ½Í£Ö¹·û0½áÊø
}



/****************************************************************************************
  * @brief:	´®¿Ú4ÖÐ¶Ï³ÌÐò£¬¸ù¾Ý½ÓÊÕ×´Ì¬£¬½ÓÊÕµ½µÄ×Ö·û·ÅÈë»º³åÇø
  			²¢ÇÒÖØÐÂÉèÖÃ½ÓÊÕ¼ÆÊ±Æ÷
  			·¢ËÍÖÐ¶Ï¸ù¾Ý¶ÁÐ´Ö¸ÕëÅÐ¶ÏÊÇ·ñ·¢ËÍÍê³É£¬Èç¹ûÍê³ÉÇåÃ¦±êÖ¾
  * @param:	none
  * @retval:	none
*****************************************************************************************/
void UART4_int (void) interrupt UART4_VECTOR	using 3
{
	if(RI4)
	{
		CLR_RI4();
		if(COM4.B_RX_EN == 1)
		{
			if(COM4.RX_Cnt >= COM_RX4_Lenth)	COM4.RX_Cnt = 0;
			RX4_Buffer[COM4.RX_Cnt++] = S4BUF;
			COM4.RX_TimeOut = TimeOutSet4;
		}
	}

	if(TI4)
	{
		CLR_TI4();
		if(COM4.TX_read != COM4.TX_write)
		{
		 	S4BUF = TX4_Buffer[COM4.TX_read];
			if(++COM4.TX_read >= COM_TX4_Lenth)		COM4.TX_read = 0;
		}
		else	COM4.B_TX_busy = 0;
	}

}

#endif

#if DEBUG_PORT == USART1

//´®¿Ú1³õÊ¼»¯
void Usart1Init(void)		//115200bps@22.1184MHz
{
	SCON = 0x50;		//8???,?????
	AUXR |= 0x40;		//???1???Fosc,?1T
	AUXR &= 0xFE;		//??1?????1???????
	TMOD &= 0x0F;		//?????1?16???????
	TL1 = 0xD0;		//??????
	TH1 = 0xFF;		//??????
	ET1 = 0;		//?????1??
	TR1 = 1;		//?????1
	
	COM1.id = 1;
	COM1.TX_read    = 0;
	COM1.TX_write   = 0;
	COM1.B_TX_busy  = 0;
	COM1.RX_Cnt     = 0;
	COM1.RX_TimeOut = 0;
	COM1.B_RX_EN    = 0;
	COM1.B_RX_OK    = 0;

	ES =0;

}

void TX1_write2buff(u8 dat)	//Ð´Èë·¢ËÍ»º³å£¬Ö¸Õë+1
{
	TX1_Buffer[COM1.TX_write] = dat;	//×°·¢ËÍ»º³å
	if(++COM1.TX_write >= COM_TX1_Lenth)	COM1.TX_write = 0;

	if(COM1.B_TX_busy == 0)		//¿ÕÏÐ
	{  
		COM1.B_TX_busy = 1;		//±êÖ¾Ã¦
		TI=1;				//´¥·¢·¢ËÍÖÐ¶Ï
	}
}


void Usart1SendString(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX1_write2buff(*puts); 	//Óöµ½Í£Ö¹·û0½áÊø
}

void Usart1SendByte(u8 dat)
{
	SBUF = dat;
	while(!TI);
	TI = 0;
}


/****************************************************************************************
  * @brief:	´®¿Ú4ÖÐ¶Ï³ÌÐò£¬¸ù¾Ý½ÓÊÕ×´Ì¬£¬½ÓÊÕµ½µÄ×Ö·û·ÅÈë»º³åÇø
  			²¢ÇÒÖØÐÂÉèÖÃ½ÓÊÕ¼ÆÊ±Æ÷
  			·¢ËÍÖÐ¶Ï¸ù¾Ý¶ÁÐ´Ö¸ÕëÅÐ¶ÏÊÇ·ñ·¢ËÍÍê³É£¬Èç¹ûÍê³ÉÇåÃ¦±êÖ¾
  * @param:	none
  * @retval:	none
*****************************************************************************************/
void UART1_int (void) interrupt UART1_VECTOR	using 3
{
	if(RI)
	{
		RI=0;
		if(COM1.B_RX_EN == 1)
		{
			if(COM1.RX_Cnt >= COM_RX1_Lenth)	COM1.RX_Cnt = 0;
			RX1_Buffer[COM1.RX_Cnt++] = SBUF;
			COM1.RX_TimeOut = TimeOutSet1;
		}
	}

	if(TI)
	{
		TI=0;
		if(COM1.TX_read != COM1.TX_write)
		{
		 	SBUF = TX1_Buffer[COM1.TX_read];
			if(++COM1.TX_read >= COM_TX1_Lenth)		COM1.TX_read = 0;
		}
		else	COM1.B_TX_busy = 0;
	}

}

#endif
