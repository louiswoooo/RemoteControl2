#include "USART.h"

COMx_Define COM2,COM4;
u8	xdata TX2_Buffer[COM_TX2_Lenth];	//发送缓冲
u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//接收缓冲
u8	xdata TX4_Buffer[COM_TX4_Lenth];	//发送缓冲
u8 	xdata RX4_Buffer[COM_RX4_Lenth];	//接收缓冲

/*******************************************************************/
void TX2_write2buff(u8 dat)	//写入发送缓冲，指针+1
{
	TX2_Buffer[COM2.TX_write] = dat;	//装发送缓冲
	if(++COM2.TX_write >= COM_TX2_Lenth)	COM2.TX_write = 0;

	if(COM2.B_TX_busy == 0)		//空闲
	{  
		COM2.B_TX_busy = 1;		//标志忙
		SET_TI2();				//触发发送中断
	}
}

void PrintString2(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX2_write2buff(*puts); 	//遇到停止符0结束
}

void TX4_write2buff(u8 dat)	//写入发送缓冲，指针+1
{
	TX4_Buffer[COM4.TX_write] = dat;	//装发送缓冲
	if(++COM4.TX_write >= COM_TX4_Lenth)	COM4.TX_write = 0;

	if(COM4.B_TX_busy == 0)		//空闲
	{  
		COM4.B_TX_busy = 1;		//标志忙
		SET_TI4();				//触发发送中断
	}
}


void PrintString4(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX4_write2buff(*puts); 	//遇到停止符0结束
}

void USART2_Config(void)		//115200bps@22.1184MHz
{
	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xD0;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	
	COM2.id = 2;
	COM2.TX_read    = 0;
	COM2.TX_write   = 0;
	COM2.B_TX_busy  = 0;
	COM2.RX_Cnt     = 0;
	COM2.RX_TimeOut = 0;
	COM2.B_RX_EN    = 0;
	COM2.B_RX_OK    = 0;
}

void USART4_Config(void)		//115200bps@22.1184MHz
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

}

/********************* UART4中断函数************************/
void UART2_int (void) interrupt UART2_VECTOR
{
	if(RI2)
	{
		CLR_RI2();
		if(COM2.B_RX_EN == 1)
		{
			if(COM2.RX_Cnt >= COM_RX2_Lenth)	COM2.RX_Cnt = 0;
			RX2_Buffer[COM2.RX_Cnt++] = S2BUF;
			COM2.RX_TimeOut = TimeOutSet2;
		}
	}
	if(TI2)
	{
		CLR_TI2();
		if(COM2.TX_read != COM2.TX_write)
		{
		 	S2BUF = TX2_Buffer[COM2.TX_read];
			if(++COM2.TX_read >= COM_TX2_Lenth)		COM2.TX_read = 0;
		}
		else	COM2.B_TX_busy = 0;
	}
}

void UART4_int (void) interrupt UART4_VECTOR
{
	if(RI4)
	{
		CLR_RI4();
		if(COM4.B_RX_EN == 1)
		{
			if(COM4.RX_Cnt >= COM_RX2_Lenth)	COM4.RX_Cnt = 0;
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


