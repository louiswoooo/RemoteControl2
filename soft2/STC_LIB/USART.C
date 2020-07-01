/*****************************************************************************
  * @file:    	usart.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-22
  * @brief:	����2���������򣬴���2����Ϊwifiģ��Ŀ��ƶ˿ڣ����ڲ��ÿ���յĽ��ջ���Ϊ:
  			ʹ�ܴ��ڽ��գ��ӽ��յ���һ�������ַ���ʼ����ʱ������յ����ַ������¿�ʼ����ʱ��
  			�����ʱ��ʱ�����жϽ��յ������ݿ飬����ȫ�������ַ��������ͨѶ��ʽ��
  			����ڽ��յ����ַ��������0���˺��ֹ���գ��ȴ���������������Ϻ��������
  			������������
  			����4���������򣬳���4�ķ��ͳ�������˻������жϷ��͵ķ�ʽ�����ͳ����Ȱ������͵���������
  			Ȼ�����������жϰ��շ��Ͷ�ָ����з��ͣ������ָ�����дָ�룬�򻺳������ݷ��ͽ���
*******************************************************************************/
#include "USART.h"

COMx_Define COM2,COM4;
u8	xdata TX2_Buffer[COM_TX2_Lenth];	//���ͻ���
u8 	xdata RX2_Buffer[COM_RX2_Lenth];	//���ջ���
u8	xdata TX4_Buffer[COM_TX4_Lenth];	//���ͻ���
u8 	xdata RX4_Buffer[COM_RX4_Lenth];	//���ջ���

/****************************************************************************************
  * @brief:	����2�ַ����ͳ����Ȱ��ַ����뻺������
  			Ȼ���ж��Ƿ񴮿����ڷ��ͣ����û�У��򴥷����ڷ���
  * @param:	dat
  * @retval:	none
*****************************************************************************************/
void TX2_write2buff(u8 dat)	//д�뷢�ͻ��壬ָ��+1
{
	TX2_Buffer[COM2.TX_write] = dat;	//װ���ͻ���
	if(++COM2.TX_write >= COM_TX2_Lenth)	COM2.TX_write = 0;

	if(COM2.B_TX_busy == 0)		//����
	{  
		COM2.B_TX_busy = 1;		//��־æ
		SET_TI2();				//���������ж�
	}
}
/****************************************************************************************
  * @brief:	����2�ַ������ͳ�������0����
  * @param:	puts
  * @retval:	none
*****************************************************************************************/
void Usart2SendString(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX2_write2buff(*puts); 	//����ֹͣ��0����
}
void TX4_write2buff(u8 dat)	//д�뷢�ͻ��壬ָ��+1
{
	TX4_Buffer[COM4.TX_write] = dat;	//װ���ͻ���
	if(++COM4.TX_write >= COM_TX4_Lenth)	COM4.TX_write = 0;

	if(COM4.B_TX_busy == 0)		//����
	{  
		COM4.B_TX_busy = 1;		//��־æ
		SET_TI4();				//���������ж�
	}
}


void Usart4SendString(u8 *puts)
{
    for (; *puts != 0;	puts++)  TX4_write2buff(*puts); 	//����ֹͣ��0����
}

//����2��ʼ��
void Usart2Init(void)		//115200bps@22.1184MHz
{
	S2CON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xD0;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
	
	COM2.id = 2;
	COM2.TX_read    = 0;
	COM2.TX_write   = 0;
	COM2.B_TX_busy  = 0;
	COM2.RX_Cnt     = 0;
	COM2.RX_TimeOut = 0;
	COM2.B_RX_EN    = 0;
	COM2.B_RX_OK    = 0;
}
//����4��ʼ��
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

}

/****************************************************************************************
  * @brief:	����2�жϳ��򣬸��ݽ���״̬�����յ����ַ����뻺����
  			�����������ý��ռ�ʱ��
  			�����жϸ��ݶ�дָ���ж��Ƿ�����ɣ���������æ��־
  * @param:	none
  * @retval:	none
*****************************************************************************************/
void UART2_int (void) interrupt UART2_VECTOR
{
	if(RI2)
	{
		CLR_RI2();
		if(COM2.B_RX_EN == 1)		//�����������
		{
			if(COM2.RX_Cnt >= COM_RX2_Lenth)	COM2.RX_Cnt = 0;	//���������壿
			RX2_Buffer[COM2.RX_Cnt++] = S2BUF;					//�յ����ַ����뻺����
			COM2.RX_TimeOut = TimeOutSet2;						//�������ó�ʱ��ʱ��
		}
	}
	if(TI2)
	{
		CLR_TI2();
		if(COM2.TX_read != COM2.TX_write)		//û����ɷ���
		{
		 	S2BUF = TX2_Buffer[COM2.TX_read];	//��������
			if(++COM2.TX_read >= COM_TX2_Lenth)		COM2.TX_read = 0;	//Խ�紦��
		}
		else	COM2.B_TX_busy = 0;			//��������
	}
}
/****************************************************************************************
  * @brief:	����4�жϳ��򣬸��ݽ���״̬�����յ����ַ����뻺����
  			�����������ý��ռ�ʱ��
  			�����жϸ��ݶ�дָ���ж��Ƿ�����ɣ���������æ��־
  * @param:	none
  * @retval:	none
*****************************************************************************************/
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

