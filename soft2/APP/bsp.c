#include "bsp.h"
#include "sys.h"
#include "string.h"
#include "wifi.h"
#include "intrins.h"
#include "exti.h"
void bsp(void)
{
	EXTI_InitTypeDef exti;

	EA=0;			//���ж�
	exti.EXTI_Interrupt = ENABLE;
	exti.EXTI_Mode = EXT_MODE_Fall;
	exti.EXTI_Polity = PolityLow;
	Ext_Inilize(EXT_INT0, &exti);

	Usart2Init();		//��ʼ������2		
	S2_Int_en();		//������2�ж�

	debug_init();		//���Կڳ�ʼ��

	IP=0x00;
	IP2=0x00;
	IP2 |=0x01;			//���ô���2 ���ȼ���
	
	
	sys_tick_init();		//ʱ��Ƭģ���ʼ��

	EA=1;				//���ж�
	DevicesInit();		//�����ʼ��

}


