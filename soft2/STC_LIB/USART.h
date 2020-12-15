
#ifndef __USART_H
#define __USART_H	 

#include	"config.h"


#define	COM_TX2_Lenth	16
#define	COM_RX2_Lenth	1800

#define	COM_TX1_Lenth	924
#define	COM_RX1_Lenth	16

#define	COM_TX4_Lenth	512
#define	COM_RX4_Lenth	128

#define	USART1	1
#define	USART2	2
#define	USART3	3
#define	USART4	4

#define	UART_ShiftRight	0		//同步移位输出
#define	UART_8bit_BRTx	(1<<6)	//8位数据,可变波特率
#define	UART_9bit		(2<<6)	//9位数据,固定波特率
#define	UART_9bit_BRTx	(3<<6)	//9位数据,可变波特率

#define	UART1_SW_P30_P31	0
#define	UART1_SW_P36_P37	(1<<6)
#define	UART1_SW_P16_P17	(2<<6)	//必须使用内部时钟
#define	UART2_SW_P10_P11	0
#define	UART2_SW_P46_P47	1

/*设置串口超时时间，以块为接收单位，如果在超时时间内
没有接收到新的字节，则认为块接收结束*/
#define	TimeOutSet1		5
#define	TimeOutSet2		5
#define	TimeOutSet3		5
#define	TimeOutSet4		5

#define	BRT_Timer1	1
#define	BRT_Timer2	2

typedef struct
{ 
	u8	id;				//串口号

	u16	TX_read;		//发送读指针
	u16	TX_write;		//发送写指针
	u8	B_TX_busy;		//忙标志

	u16 	RX_Cnt;			//接收字节计数
	u16	RX_TimeOut;		//接收超时
	u8	B_RX_EN;		//允许接收块
	u8	B_RX_OK;		//允许接收块
	
} COMx_Define; 

typedef struct
{ 
	u8	UART_Mode;			//模式,         UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	u8	UART_BRT_Use;		//使用波特率,   BRT_Timer1,BRT_Timer2
	u32	UART_BaudRate;		//波特率,       ENABLE,DISABLE
	u8	Morecommunicate;	//多机通讯允许, ENABLE,DISABLE
	u8	UART_RxEnable;		//允许接收,   ENABLE,DISABLE
	u8	BaudRateDouble;		//波特率加倍, ENABLE,DISABLE
	u8	UART_Interrupt;		//中断控制,   ENABLE,DISABLE
	u8	UART_Polity;		//优先级,     PolityLow,PolityHigh
	u8	UART_P_SW;			//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	u8	UART_RXD_TXD_Short;	//内部短路RXD与TXD, 做中继, ENABLE,DISABLE

} COMx_InitDefine; 

extern	COMx_Define	COM2;
extern	u8	xdata TX2_Buffer[COM_TX2_Lenth];	//发送缓冲
extern	u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//接收缓冲

extern	COMx_Define	COM4;
extern	u8	xdata TX4_Buffer[COM_TX4_Lenth];	//发送缓冲
extern	u8 	xdata RX4_Buffer[COM_RX4_Lenth];	//接收缓冲

extern	COMx_Define	COM1;
extern	u8	xdata TX1_Buffer[COM_TX1_Lenth];	//发送缓冲
extern	u8 	xdata RX1_Buffer[COM_RX1_Lenth];	//接收缓冲

/************************Below is personal adding drives*********************************/
void Usart2Init (void);
void Usart2SendString(u8 *puts);
void TX2_write2buff(u8 dat);


void Usart4Init(void);
void Usart4SendString(u8 *puts);
void Usart1Init(void);
void Usart1SendString(u8 *puts);

#endif



