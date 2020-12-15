
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

#define	UART_ShiftRight	0		//ͬ����λ���
#define	UART_8bit_BRTx	(1<<6)	//8λ����,�ɱ䲨����
#define	UART_9bit		(2<<6)	//9λ����,�̶�������
#define	UART_9bit_BRTx	(3<<6)	//9λ����,�ɱ䲨����

#define	UART1_SW_P30_P31	0
#define	UART1_SW_P36_P37	(1<<6)
#define	UART1_SW_P16_P17	(2<<6)	//����ʹ���ڲ�ʱ��
#define	UART2_SW_P10_P11	0
#define	UART2_SW_P46_P47	1

/*���ô��ڳ�ʱʱ�䣬�Կ�Ϊ���յ�λ������ڳ�ʱʱ����
û�н��յ��µ��ֽڣ�����Ϊ����ս���*/
#define	TimeOutSet1		5
#define	TimeOutSet2		5
#define	TimeOutSet3		5
#define	TimeOutSet4		5

#define	BRT_Timer1	1
#define	BRT_Timer2	2

typedef struct
{ 
	u8	id;				//���ں�

	u16	TX_read;		//���Ͷ�ָ��
	u16	TX_write;		//����дָ��
	u8	B_TX_busy;		//æ��־

	u16 	RX_Cnt;			//�����ֽڼ���
	u16	RX_TimeOut;		//���ճ�ʱ
	u8	B_RX_EN;		//������տ�
	u8	B_RX_OK;		//������տ�
	
} COMx_Define; 

typedef struct
{ 
	u8	UART_Mode;			//ģʽ,         UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	u8	UART_BRT_Use;		//ʹ�ò�����,   BRT_Timer1,BRT_Timer2
	u32	UART_BaudRate;		//������,       ENABLE,DISABLE
	u8	Morecommunicate;	//���ͨѶ����, ENABLE,DISABLE
	u8	UART_RxEnable;		//�������,   ENABLE,DISABLE
	u8	BaudRateDouble;		//�����ʼӱ�, ENABLE,DISABLE
	u8	UART_Interrupt;		//�жϿ���,   ENABLE,DISABLE
	u8	UART_Polity;		//���ȼ�,     PolityLow,PolityHigh
	u8	UART_P_SW;			//�л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(����ʹ���ڲ�ʱ��)
	u8	UART_RXD_TXD_Short;	//�ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE

} COMx_InitDefine; 

extern	COMx_Define	COM2;
extern	u8	xdata TX2_Buffer[COM_TX2_Lenth];	//���ͻ���
extern	u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//���ջ���

extern	COMx_Define	COM4;
extern	u8	xdata TX4_Buffer[COM_TX4_Lenth];	//���ͻ���
extern	u8 	xdata RX4_Buffer[COM_RX4_Lenth];	//���ջ���

extern	COMx_Define	COM1;
extern	u8	xdata TX1_Buffer[COM_TX1_Lenth];	//���ͻ���
extern	u8 	xdata RX1_Buffer[COM_RX1_Lenth];	//���ջ���

/************************Below is personal adding drives*********************************/
void Usart2Init (void);
void Usart2SendString(u8 *puts);
void TX2_write2buff(u8 dat);


void Usart4Init(void);
void Usart4SendString(u8 *puts);
void Usart1Init(void);
void Usart1SendString(u8 *puts);

#endif



