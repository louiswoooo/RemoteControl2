#include "bsp.h"
#include "sys.h"
#include "string.h"
#include "wifi.h"
void bsp(void)
{

	EA=0;			//���ж�
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

