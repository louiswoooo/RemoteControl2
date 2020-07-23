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
#include "http_ap.h"
#define SERVER_ADDR		"119.3.233.56"
#define SERVER_PORT		8000

#define WIFI_CMD_TIMEOUT_MS	200
#define WIFI_CMD_TRY_TIMES		5

#define WIFI_SEND_TIMEOUT_MS	500
#define WIFI_SEND_TRY_TIMES	3

typedef	struct	
{
	u8 *send;			//����
	u8 *match;			//ƥ��
}
_t_WIFI_CMD_Info;

/*****************************************************************************
ESP8266���ñ���
*******************************************************************************/
//�ָ���������
const _t_WIFI_CMD_Info	CMD_Restore={"AT+RESTORE\r\n", "OK"};                
//##����ESP8266ģ��ΪAPģʽ������wifi�ȵ㡣
const _t_WIFI_CMD_Info	CMD_APMode={"AT+CWMODE=2\r\n", "OK"};   
//wifi����ESP8266�����룺123456��4�Ǽ��ܷ�ʽ��4���ŵ�
const _t_WIFI_CMD_Info	CMD_SSID={"AT+CWSAP=\"AI_Lab\",\"1234567890\",4,4\r\n", "OK"};                
//##����ģ��ʹAPģʽ��Ч
const _t_WIFI_CMD_Info	CMD_Reset={"AT+RST\r\n", "OK"};                      
//## 1���������ӣ�0�ر�
//����ǿ��һ�£�ESP8266��Ϊ����������֧��5���ͻ��˵����ӣ�id����˳����0-4��
const _t_WIFI_CMD_Info	CMD_Muti={"AT+CIPMUX=1\r\n", "OK"};                
//##ģ�鿪��������ģʽ���˿ں�8080
const _t_WIFI_CMD_Info	CMD_Server={"AT+CIPSERVER=1,5000\r\n", "OK"};        
//##�鿴һ��ESP8266��IP
const _t_WIFI_CMD_Info	CMD_IP={"AT+CIFSR\r\n", "OK"};        

//apģʽ�µķ������0���ӿͻ��ˣ�170�����ֽڳ���
_t_WIFI_CMD_Info 	CMD_Send= {"AT+CIPSEND=0,170\r\n", "OK"};


#define WIFI_GPIO_PORT		GPIO_P5
#define WIFI_GPIO_PIN		GPIO_Pin_3

#define	wifi_reset_pin_low()		GPIO_ClearBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)
#define	wifi_reset_pin_high()		GPIO_SetBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)

/*
  * @brief:	��������ת�����ַ�������
  * @param:	num Ҫת����������str ת���������ַ����Ĵ����ؼ�ָ��
  * @retval:	����ȥ��ǰ���0�������ַ���ָ��
*/
u8 *int_to_str( u8 *str,u16 num)
{
	u8 *p;
	str[0] = num/10000+0x30;
	str[1] = num%10000/1000+0x30;
	str[2] = num%1000/100+0x30;
	str[3] = num%100/10+0x30;
	str[4] = num%10+0x30;
	str[5] = '\0';
	p=str;
	while(*p == '0')
		p++;
	if(*p == 0)
		p++;
	return p;
}

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
	delay_s(2);
}
/****************************************************************************************
  * @brief:	esp8266 ���ڽ��պ������������գ��Կ�Ϊ��λ���մ�����Ϣ��
			���ú����Ժ����TimeOutSet2*(ms/tick) ʱ���ڽ��ղ������ֽ�
			���տ���������ؽ��յ����ֽ�����������Ϊ��������	
			����ʱ��Ϊ: ���տ�ʱ��+ TimeOutSet2
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
	COM2.B_RX_EN=0;
	if(COM2.B_RX_OK)
	{
		return COM2.RX_Cnt;
	}
	return NULL;
}


/****************************************************************************************
  * @brief:	��esp8266 ���������ƥ�仺�������е��ַ�				
  * @param:	send:	Ҫ���͵�����
			match:	Ҫƥ����ַ���
			timeout_ticks:	���ó�ʱ��ʱ��ms
  * @retval:	����ڳ�ʱʱ�����յ�������Ϣ��ÿ����Ϣ����match ƥ�䣬
  			�ɹ���������1����ʱ�򷵻�0
*****************************************************************************************/

u8 WIFI_SendAndWait(u8 *send, u8 *match, u16 timeout_ms)
{
	u8 *p;
	u8 i;
	Usart2SendString(send);			//��������
	for(i = timeout_ms/(TimeOutSet2 * MS_PER_TICK); i>0; i--)		//���㳬ʱ����
	{
		if(wifi_receive())			//�ȴ�����Ϣ
		{
			debug(RX2_Buffer);
			if(p=strstr(RX2_Buffer,match))
			{
				debug_vip("match");
				return 1;
			}
		}
	}
	return 0;
}
/****************************************************************************************
  * @brief:	�ͻ������� ���ͺ������ȷ���׼�����͵����AT+CIPSEND=clientid,length	��
  			������client�ĺţ�0-4�����ͷ������ݵĳ��ȣ�Ȼ��������ݣ�
  			���length����ʵ�ʷ��͵����ݳ��ȣ���˴η��Ͳ�������
  			���lengthС��ʵ�ʷ��͵ĳ��ȣ���˴η���length���ȵ����ݣ���������ݽض϶�ʧ
  * @param:	client_id�����ӵĿͻ���id�� p��������ָ��
  * @retval:	�ɹ�����1��ʧ��0
*****************************************************************************************/
u8 send_to_client(u8 *client_id, u8 *p)
{
	u8 temp[6];
	u8 cmd[30]="AT+CIPSEND=";
	u8 *str;
	u16 send_len = strlen(p);
	strcat(cmd, client_id);				//��Ͽͻ���id
	strcat(cmd, ",");
	str = int_to_str(temp, send_len);
	strcat(cmd, str);					//��ϳ���
	strcat(cmd, "\r\n");

	if( !WIFI_SendAndWait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))		//���ͷ�������
		return FAIL;
	if( !WIFI_SendAndWait(p, "SEND OK", WIFI_SEND_TIMEOUT_MS))			//��������
		return FAIL;

	return SUCCESS;
}
/****************************************************************************************
  * @brief:	http�ͺ������ȷ��ͷ���http ͷ��Ȼ����content_length�����content
  * @param:	client_id�����ӵĿͻ���id�� p��������ָ��
  * @retval:	�ɹ�����1��ʧ��0
*****************************************************************************************/
u8 http_send(u8 *client_id, u8 *content)
{
	u8 http_content_lenth_string[10];
	u8 temp[6];
	u8 *p;
	u16 content_size = strlen(content);
	
	memset(http_content_lenth_string, 0, sizeof(http_content_lenth_string));
	if(!send_to_client(client_id, HTTP_H1) )		//����http ͷ��������content_length
		return FAIL;
	
	p = int_to_str( temp, content_size);			//����content_length���������з�
	strcat(http_content_lenth_string, p);
	strcat(http_content_lenth_string,"\r\n\r\n");
	if(!send_to_client(client_id, http_content_lenth_string))
		return FAIL;
	
	if(!send_to_client(client_id, content))		//��������
		return FAIL;
	return SUCCESS;

}



u8 WIFI_Set_AP_mode(void)
{
	if( !WIFI_SendAndWait(CMD_APMode.send, CMD_APMode.match, WIFI_CMD_TIMEOUT_MS))
		return FAIL;
	if( !WIFI_SendAndWait(CMD_SSID.send, CMD_SSID.match, WIFI_CMD_TIMEOUT_MS ))
	{
		delay_s(2);
		return FAIL;
	}
	if( !WIFI_SendAndWait(CMD_Reset.send, CMD_Reset.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	delay_s(2);
	if( !WIFI_SendAndWait(CMD_Muti.send, CMD_Muti.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	if( !WIFI_SendAndWait(CMD_Server.send, CMD_Server.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	if( !WIFI_SendAndWait(CMD_IP.send, CMD_IP.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	return SUCCESS;
}


