/*****************************************************************************
  * @file:    	debug.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-6-6
  * @brief:	����ģ�飬��Ҫ���ô��ڽ��е�����Ϣ�����
	�����ַ�������� unsigned int��2�ֽڵı������
*******************************************************************************/
#include "config.h"
#include "debug.h"
#include "usart.h"

/****************************************************************************************
  * @brief:	����һ��u16����
	* @param:	var: u16����
  * @retval:	none
*****************************************************************************************/
/*
void DebugVar(u16 var)
{
	u8 num[6];
	num[0]=var/10000+0x30;
	num[1]=var%10000/1000+0x30;
	num[2]=var%1000/100+0x30;
	num[3]=var%100/10+0x30;
	num[4]=var%10+0x30;
	num[5]='\0';
	debug(num);
}
*/