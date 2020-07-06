/*****************************************************************************
  * @file:    	wifi.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-4-17
  * @brief:	����wifi ģ��esp8266 ��Ϊ�ϲ��ṩ�ӿ�
*******************************************************************************/
#include "usart.h"
#include "string.h"
#include "gpio.h"
#include "sys.h"
#include "wifi.h"

#define SERVER_ADDR		"119.3.233.56"
#define SERVER_PORT		8000

typedef	struct	
{
	u8 *send;			//����
	u8 *match;			//ƥ��
	u8 timeout_ticks;	//��ʱ
}
_t_WIFI_CMD_Info;

/*****************************************************************************
����˵��:
@AP_Para1:	����ESP8266ģ��ΪAPģʽ������wifi�ȵ㡣
@AP_Para2:	����ģ��ʹAPģʽ��Ч
@AP_Para3:	���������ӣ�ESP8266��Ϊ���������
			֧��5���ͻ��˵����ӣ�id����˳����0-4��
@AP_Para4:	ģ�鿪��������ģʽ���˿ں�8080
*******************************************************************************/
const _t_WIFI_CMD_Info	AP_Para0={"AT+RESTORE\r\n", "OK", 300};                
const _t_WIFI_CMD_Info	AP_Para1={"AT+CWMODE=2\r\n", "OK", 300};                
const _t_WIFI_CMD_Info	AP_Para2={"AT+CWSAP=\"AI_Lab\",\"1234567890\",4,4\r\n", "OK", 300};                
const _t_WIFI_CMD_Info	AP_Para3={"AT+RST\r\n", "OK", 300};                      
const _t_WIFI_CMD_Info	AP_Para4={"AT+CIPMUX=1\r\n", "OK", 300};                
const _t_WIFI_CMD_Info	AP_Para5={"AT+CIPSERVER=1,8000\r\n", "OK", 300};        
const _t_WIFI_CMD_Info	AP_Para6={"AT+CIFSR\r\n", "OK", 300};        


#define WIFI_GPIO_PORT		GPIO_P4
#define WIFI_GPIO_PIN		GPIO_Pin_2

#define	wifi_reset_pin_low()		GPIO_ClearBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)
#define	wifi_reset_pin_high()		GPIO_SetBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)

//esp8266Ӳ������
void wifi_reset(void)
{
	GPIO_InitTypeDef	wifi_gpio_init;

	wifi_gpio_init.Mode=GPIO_PullUp;
	wifi_gpio_init.Pin=WIFI_GPIO_PIN;
	
	GPIO_Inilize(WIFI_GPIO_PORT, &wifi_gpio_init);

	wifi_reset_pin_low();
	delay_ms(100);
	wifi_reset_pin_high();
	delay_s(5);
}

/****************************************************************************************
  * @brief:	esp8266 ���ڽ��պ������������գ��Կ�Ϊ��λ���մ�����Ϣ��
			���ú����Ժ����TimeOutSet2 ʱ���ڽ��ղ������ֽ�
			���տ���������ؽ��յ����ֽ�����������Ϊ��������	
  * @param:	none
  * @retval:	���ؽ��յ����ֽ��������յ��Ŀ���ڻ���RX2_Buffer
*****************************************************************************************/
u8 wifi_receive()
{
	COM2.B_RX_EN=1;
	COM2.RX_Cnt=0;
	COM2.B_RX_OK=0;
	COM2.RX_TimeOut=TimeOutSet2;
	while(COM2.RX_TimeOut>0);			//ÿ�δ��ڽ��ղ����ֽڲ�����ʱ���ж�Ϊһ�ο�������
	if(COM2.B_RX_OK)
	{
		COM2.B_RX_EN=0;
		return COM2.RX_Cnt;
	}
	return NULL;
}

/****************************************************************************************
  * @brief:	��esp8266 ���������ƥ�仺�������е��ַ�				
  * @param:	send:	Ҫ���͵�����
			match:	Ҫƥ����ַ���
			timeout_ticks:	���ó�ʱ��ticks
  * @retval:	�����match ƥ�䣬�򷵻�ƥ��ָ�룬���򷵻�NULL
*****************************************************************************************/

u8 *WIFI_SendAndWait(u8 *send, u8 *match, u16 timeout_ticks)
{
	u8 *p;
	u16 i;
	for(i = timeout_ticks/TimeOutSet2; i>0; i--)
	{
		wifi_send(send);
		debug(">>>>");
		debug(send);
		if(wifi_receive())
		{
			debug(RX2_Buffer);
			if(p=strstr(RX2_Buffer,match))
				return 1;
		}
	}
	return 0;
}

u8 WIFI_Set_AP_mode(void)
{
	/*
	if( !WIFI_SendAndWait(AP_Para0.send, AP_Para0.match, AP_Para0.timeout_ticks))
		return FAIL;
	delay_s(5);
	*/
	if( !WIFI_SendAndWait(AP_Para1.send, AP_Para1.match, AP_Para1.timeout_ticks))
		return FAIL;
	if( !WIFI_SendAndWait(AP_Para2.send, AP_Para2.match, AP_Para2.timeout_ticks))
	{
		delay_s(2);
		return FAIL;
	}
	if( !WIFI_SendAndWait(AP_Para3.send, AP_Para3.match, AP_Para3.timeout_ticks))
		return FAIL;
	delay_s(5);
	if( !WIFI_SendAndWait(AP_Para4.send, AP_Para4.match, AP_Para4.timeout_ticks))
		return FAIL;
	if( !WIFI_SendAndWait(AP_Para5.send, AP_Para5.match, AP_Para5.timeout_ticks))
		return FAIL;
	if( !WIFI_SendAndWait(AP_Para6.send, AP_Para6.match, AP_Para6.timeout_ticks))
		return FAIL;
	return SUCCESS;
}

/*
u8 WIFI_Set_Client_mode(void)
{
	

}
*/

