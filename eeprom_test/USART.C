/*****************************************************************************
  * @file:    	usart.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-22
  * @brief:	串口2的驱动程序，串口2设置为wifi模块的控制端口，串口采用块接收的接收机制为:
  			使能串口接收，从接收到第一个串口字符开始倒计时，如果收到新字符则重新开始倒计时，
  			如果计时超时，则判断接收到了数据块，由于全部采用字符串命令的通讯方式，
  			最后在接收到的字符串后面加0，此后禁止接收，等待任务处理该命令完毕后，任务可以
  			重新启动接收
  			串口4的驱动程序，出口4的发送程序采用了缓冲区中断发送的方式，发送程序先把数据送到缓冲区，
  			然后启动触发中断按照发送读指针进行发送，如果读指针等于写指针，则缓冲区数据发送结束
*******************************************************************************/
#include "USART.h"

COMx_Define COM2;
u8	xdata TX2_Buffer[COM_TX2_Lenth];	//发送缓冲
u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//接收缓冲

#if DEBUG_PORT==USART4
COMx_Define COM4;
u8	xdata TX4_Buffer[COM_TX4_Lenth];	//发送缓冲
u8 	xdata RX4_Buffer[COM_RX4_Lenth];	//接收缓冲
#else
COMx_Define COM1;
u8	xdata TX1_Buffer[COM_TX1_Lenth];	//发送缓冲
u8 	xdata RX1_Buffer[COM_RX1_Lenth];	//接收缓冲
#endif


//串口2初始化
void Usart2Init(void)		//115200bps@22.1184MHz
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


/****************************************************************************************
  * @brief:	串口2字符发送程序，先把字符存入缓冲区，
  			然后判断是否串口正在发送，如果没有，则触发串口发送
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
  * @brief:	串口2字符串发送程序，遇到0结束
  * @param:	puts
  * @retval:	none
*****************************************************************************************/
void Usart2SendString(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX2_write2buff(*puts); 	//遇到停止符0结束
}

/****************************************************************************************
  * @brief:	串口2中断程序，根据接收状态，接收到的字符放入缓冲区
  			并且重新设置接收计时器
  			发送中断根据读写指针判断是否发送完成，如果完成清忙标志
  * @param:	none
  * @retval:	none
*****************************************************************************************/
void UART2_int (void) interrupt UART2_VECTOR		using 2
{
	if(RI2)
	{
		CLR_RI2();
		if(COM2.B_RX_EN == 1)		//如果允许接收
		{
			if(COM2.RX_Cnt >= COM_RX2_Lenth)	COM2.RX_Cnt = 0;	//缓冲区溢出澹�
			RX2_Buffer[COM2.RX_Cnt++] = S2BUF;					//收到的字符放入缓冲区
			COM2.RX_TimeOut = TimeOutSet2;						//重新设置超时计时器
		}
	}
}

#if DEBUG_PORT == USART4

//串口4初始化
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

void Usart4SendString(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX4_write2buff(*puts); 	//遇到停止符0结束
}



/****************************************************************************************
  * @brief:	串口4中断程序，根据接收状态，接收到的字符放入缓冲区
  			并且重新设置接收计时器
  			发送中断根据读写指针判断是否发送完成，如果完成清忙标志
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

//串口1初始化
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

void TX1_write2buff(u8 dat)	//写入发送缓冲，指针+1
{
	TX1_Buffer[COM1.TX_write] = dat;	//装发送缓冲
	if(++COM1.TX_write >= COM_TX1_Lenth)	COM1.TX_write = 0;

	if(COM1.B_TX_busy == 0)		//空闲
	{  
		COM1.B_TX_busy = 1;		//标志忙
		TI=1;				//触发发送中断
	}
}


void Usart1SendString(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX1_write2buff(*puts); 	//遇到停止符0结束
}

void Usart1SendByte(u8 dat)
{
	SBUF = dat;
	while(!TI);
	TI = 0;
}


/****************************************************************************************
  * @brief:	串口4中断程序，根据接收状态，接收到的字符放入缓冲区
  			并且重新设置接收计时器
  			发送中断根据读写指针判断是否发送完成，如果完成清忙标志
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
