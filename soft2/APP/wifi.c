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
#include "http_text.h"

#define WIFI_CMD_TIMEOUT_MS	3000
#define WIFI_CMD_TRY_TIMES		5

#define WIFI_SEND_TIMEOUT_MS	3000
#define WIFI_SEND_TRY_TIMES	3

#define SERVER_RESPONSE_BLOCK_SIZE	1800

typedef	struct	
{
	u8 *send;			//����
	u8 *match;			//ƥ��
}
_t_WIFI_CMD_Info;

/*****************************************************************************
����ESP8266 �����ģʽ����ʹ𸴹ؼ���
��������:����APģʽ-����ssid������-����-���ö�����-����������-�鿴ip
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


/*****************************************************************************
����ESP8266 �ͻ��� ģʽ����ʹ𸴹ؼ���
��������:����STAģʽ-����-����wifi-����������-����TCP����
*******************************************************************************/
//##����ESP8266ģ��ΪStation ģʽ������wifi�ȵ㡣
const _t_WIFI_CMD_Info	CMD_STAMode={"AT+CWMODE=1\r\n", "OK"};   
//����·������wifi����HiTV_woo�����룺1234567890��
const _t_WIFI_CMD_Info	CMD_Connect_WIFI={"AT+CWJAP=\"HiTV_woo_plus\",\"1234567890\"\r\n", "OK"};                
//����������
const _t_WIFI_CMD_Info	CMD_Single={"AT+CIPMUX=0\r\n", "OK"};                
//����TCP ����
const _t_WIFI_CMD_Info	CMD_Connect_Server={"AT+CIPSTART=\"TCP\",\"192.168.1.106\",5000\r\n", "OK"};        
//�ر�TCP ����
const _t_WIFI_CMD_Info	CMD_Disconnect_Server={"AT+CIPCLOSE=0\r\n", "OK"};        

#define WIFI_GPIO_PORT		GPIO_P5
#define WIFI_GPIO_PIN		GPIO_Pin_3

#define	wifi_reset_pin_low()		GPIO_ClearBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)
#define	wifi_reset_pin_high()		GPIO_SetBit(WIFI_GPIO_PORT, WIFI_GPIO_PIN)

/*****************************************************************************
  * @brief:	��������ת�����ַ�������
  * @param:	num Ҫת����������str ת���������ַ����Ĵ����ؼ�ָ��
  * @retval:	����ȥ��ǰ���0�������ַ���ָ��
*******************************************************************************/
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

/*****************************************************************************
  * @brief:	esp 8266 Ӳ����������Ҫ����һ���͵�ƽ��Ȼ����ʱ�������
  * @param:	num Ҫת����������str ת���������ַ����Ĵ����ؼ�ָ��
  * @retval:	����ȥ��ǰ���0�������ַ���ָ��
*******************************************************************************/
void WIFI_Reset(void)
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
			���ú����Ժ�ÿ����(TimeOutSet2* MS_PER_TICK) ʱ����մ�����Ϣ��
			����յ��򷵻أ�����ȴ�timeout_ms ʱ����Ȼû���յ�һ���ֽ�
			������ʱ����
			����COM2.RX_TimeOut Ϊ����ճ�ʱ��������ÿ��tick �жϻ�-1
  * @param:	timeout_ms, wifi ���յĳ�ʱʱ��
  * @retval:	���ؽ��յ����ֽ��������յ��Ŀ���ڻ���RX2_Buffer
*****************************************************************************************/
u8 WIFI_Receive(u16 timeout_ms)	
{
	u16 i;
	if(timeout_ms > 0)
	{
		for(i= timeout_ms/(TimeOutSet2* MS_PER_TICK); i>0; i--)		//������timeout_msʱ���ѭ������
		{
			COM2.B_RX_EN=1;		//�����������
			COM2.RX_Cnt=0;			//������ռ�����
			COM2.B_RX_OK=0;		//������ձ�־
			COM2.RX_TimeOut=TimeOutSet2;	//���ÿ����ʱ��
			while(COM2.RX_TimeOut>0);			//�������ճ�ʱʱ��δ���������ȴ�
			COM2.B_RX_EN=0;			//��ʱʱ�䵽��ֹͣ����
			if(COM2.B_RX_OK)			//����յ��ֽ�
			{
				return COM2.RX_Cnt;	//�����ֽ�
			}
		}
	}
	return NULL;
}


/****************************************************************************************
  * @brief:	��esp8266 ������Ϣ����ƥ�仺�������е��ַ�
  			�ȴ�timeout_ms ����յ�ƥ���򷵻�1�����򷵻�0
  * @param:	send:	Ҫ���͵�����
			match:	Ҫƥ����ַ���
			timeout_ms:	���ó�ʱ��ʱ��ms
  * @retval:	����ڳ�ʱʱ�����յ�������Ϣ��ÿ����Ϣ����match ƥ�䣬
  			�ɹ���������1����ʱ�򷵻�0
*****************************************************************************************/
u8 wifi_send_and_wait(u8 *send, u8 *match, u16 timeout_ms)
{
	u8 *p;
	u16 i;
	
	Usart2SendString(send);			//������Ϣ
	for(i = (timeout_ms/TimeOutSet2) / MS_PER_TICK; i>0; i--)		
	{
		if(WIFI_Receive(TimeOutSet2 * MS_PER_TICK))			//�ȴ�����Ϣ
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
  * @brief:	esp8266��Ϊ����˵�http response �������յ�http�����Ժ�
  			���øú�������response (�ο�HTTP Э��)
  			�ֳ�3���ַ��ͣ��ȷ���http ͷ��Ȼ����content_length�����content
  * @param:	client_id�����ӵĿͻ���id�� p��������ָ��
  * @retval:	�ɹ�����1��ʧ��0
*****************************************************************************************/
u8 WIFI_Server_HTTP_Response(u8 *client_id, u8 *content)
{
	u16 i, j;
	u8 temp[6];
	u8 response_head2[12];
	u8 cmd[30]="AT+CIPSEND=";	//��������
	u8 *str;
	u16 len, content_size;	
	u8 *p;

	content_size = strlen(content);
	//����h1+h2
	str = int_to_str(temp, content_size);
	response_head2[0] = '\0';
	strcat(response_head2, str);
	strcat(response_head2, "\r\n\r\n");
	len = strlen(HTTP_Server_Response_Head1) + strlen(response_head2);	//���㷢�ͳ���
	str = int_to_str(temp, len);		//ת���ɳ����ַ�
	strcat(cmd, client_id);
	strcat(cmd, ",");
	strcat(cmd, str);					//��ϳ���
	strcat(cmd, "\r\n");
	if(!wifi_send_and_wait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))	//���ͷ�������ȴ���Ӧ
		return FAIL;
	Usart2SendString(HTTP_Server_Response_Head1);
	Usart2SendString(response_head2);
	if(!wifi_send_and_wait(NULL, "SEND OK", WIFI_SEND_TIMEOUT_MS))
		return FAIL;
	//����content
	str = int_to_str(temp, SERVER_RESPONSE_BLOCK_SIZE);		//ת���ɳ����ַ�
	p = content;
	for(i = content_size/SERVER_RESPONSE_BLOCK_SIZE;i>0;i--)
	{
		debug(">>>");
		debug_var(i);
		debug("\r\n");
		cmd[0] = '\0';
		strcat(cmd,"AT+CIPSEND=");
		strcat(cmd, client_id);
		strcat(cmd, ",");
		strcat(cmd, str);					//��ϳ���
		strcat(cmd, "\r\n");
		if(!wifi_send_and_wait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))	//��������ȴ���Ӧ
			return FAIL;
		for(j=SERVER_RESPONSE_BLOCK_SIZE; j > 0; j--)
		{
			TX2_write2buff(*p);
			p++;
		}
		if(!wifi_send_and_wait(0x00, "SEND OK", WIFI_SEND_TIMEOUT_MS))
			return FAIL;
	}
	//���ͽ�β
	str = int_to_str(temp, strlen(p));
	cmd[0] = '\0';
	strcat(cmd,"AT+CIPSEND=");
	strcat(cmd, client_id);
	strcat(cmd, ",");
	strcat(cmd, str);					//��ϳ���
	strcat(cmd, "\r\n");
	if(!wifi_send_and_wait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))	//��������ȴ���Ӧ
		return FAIL;
	if(!wifi_send_and_wait(p, "SEND OK", WIFI_SEND_TIMEOUT_MS))
		return FAIL;
	return SUCCESS;

}
/****************************************************************************************
  * @brief:	esp8266��Ϊ�ͻ��˵�http request ���������ͷ�����http����
  			���øú�������request (�ο�HTTP Э��)
  			�ֳ�4���ַ���:
  			1. ����http ͷǰ��,����: GET /a11?
  			2. �����������SWITH1=ON&SWITCH2=OFF&xxxx��
  			3. ����ͷʣ����Ϣ
  			4. ���Ͳ���2�Ĳ���
  * @param:	h1 http ͷǰ��, para http������h3ͷ��ʣ��
  * @retval:	�ɹ�����1��ʧ��0
*****************************************************************************************/
u8 WIFI_Client_HTTP_Request(u8 *h1, u8 *para, u8 *h3)
{
	u8 temp[6];
	u8 cmd[30]="AT+CIPSEND=";	//��������
	u8 *str;
	u16 request_len = strlen(h1) + strlen(para)*2 + strlen(h3) ;	//���㳤��
	str = int_to_str(temp, request_len);		//ת���ɳ����ַ�
	strcat(cmd, str);					//��ϳ���
	strcat(cmd, "\r\n");

	if(!wifi_send_and_wait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))	//���ͷ�������ȴ���Ӧ
		return FAIL;
	Usart2SendString(h1);	//����ͷ��1
	Usart2SendString(para);	//���Ͳ���
	Usart2SendString(h3);	//����ͷ��3
	if(!wifi_send_and_wait(para, "SEND OK", WIFI_CMD_TIMEOUT_MS ))		//���Ͳ��������ȴ��������
		return FAIL;
	debug_vip("SEND OK");
	return SUCCESS;
}
/*********************************************************************************
����esp8266 Ϊ������ģʽ������wifi�ȵ㣬���Ӻ���Խ��б��ؿ��ƺ�����
��������:����APģʽ-����ssid������-����-���ö�����-����������-�鿴ip
*******************************************************************************/
u8 WIFI_SetServerMode(void)
{
	//����ΪAPģʽ
	if( !wifi_send_and_wait(CMD_APMode.send, CMD_APMode.match, WIFI_CMD_TIMEOUT_MS))
		return FAIL;
	//����wifi �ȵ���Ϣ
	if( !wifi_send_and_wait(CMD_SSID.send, CMD_SSID.match, WIFI_CMD_TIMEOUT_MS ))
	{
		return FAIL;
	}
	//����
	if( !wifi_send_and_wait(CMD_Reset.send, CMD_Reset.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	delay_s(2);		//�����ȴ�
	//���ö����ӣ�������ģʽ��������Ϊ������
	if( !wifi_send_and_wait(CMD_Muti.send, CMD_Muti.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	//���÷�����ģʽ�Ͷ˿�
	if( !wifi_send_and_wait(CMD_Server.send, CMD_Server.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	//�鿴ip
	if( !wifi_send_and_wait(CMD_IP.send, CMD_IP.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	return SUCCESS;
}
/*****************************************************************************
����ESP8266 �ͻ��� ģʽ����ʹ𸴹ؼ���
��������:����STAģʽ-����-����wifi-����������
*******************************************************************************/
u8 WIFI_SetClientMode(void)
{
	//����Ϊstation ģʽ
	if( !wifi_send_and_wait(CMD_STAMode.send, CMD_STAMode.match, WIFI_CMD_TIMEOUT_MS))
		return FAIL;
	if( !wifi_send_and_wait(CMD_Reset.send, CMD_Reset.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	delay_s(2);
	//����wifi��ע��wifi������ʱ�ϳ������Ը��˽ϳ���ʱʱ��
	if( !wifi_send_and_wait(CMD_Connect_WIFI.send, CMD_Connect_WIFI.match, 20000 ))	
	{
		return FAIL;
	}
	//����Ϊ������ģʽ
	if( !wifi_send_and_wait(CMD_Single.send, CMD_Single.match, WIFI_CMD_TIMEOUT_MS ))
		return FAIL;
	return SUCCESS;
}
/*****************************************************************************
  * @brief:	�ͻ���ģʽ�����ӷ�����
  		��������: AT+CIPSTART="TCP","192.168.1.106",5000\r\n
  * @param:	ip ���ӵķ�����ip , port �������˿�
  * @retval:	�ɹ�����1��ʧ��0
*******************************************************************************/
u8 WIFI_ClientConnectServer(u8 *ip, u8 *port)
{
	u8 cmd[50] = {"AT+CIPSTART=\"TCP\",\""};
	strcat(cmd, ip);
	strcat(cmd, "\",");
	strcat(cmd, port);
	strcat(cmd, "\r\n");
	if( !wifi_send_and_wait(cmd, "CONNECT", 10000 ))
		return FAIL;
	return SUCCESS;
}

/****************************************************************************************
  * @brief:	esp8266��Ϊ����������ӵĿͻ��˷��ͺ�������ʽ����:
  			AT+CIPSEND=clientid,length		//client ���ӵĿͻ���id��length ���͵����ݳ���
  			OK
  			>xxxxxxx				//���͵�����
  			busy s...
  			Recv xx bytes
  			SEND OK
  			
  			���length����ʵ�ʷ��͵����ݳ��ȣ���˴η��Ͳ�������
  			���lengthС��ʵ�ʷ��͵ĳ��ȣ���˴η���length���ȵ����ݣ���������ݽض϶�ʧ
  * @param:	client_id�����ӵĿͻ���id�� p��������ָ��
  * @retval:	�ɹ�����1��ʧ��0
*****************************************************************************************
u8 server_send_to_client(u8 *client_id, u8 *p)
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

	if( !wifi_send_and_wait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))		//���ͷ�������
		return FAIL;
	if( !wifi_send_and_wait(p, "SEND OK", WIFI_SEND_TIMEOUT_MS))			//��������
		return FAIL;

	return SUCCESS;
}
****************************************************************************************
  * @brief:	esp8266��Ϊ�ͻ��������ӵķ���˷��ͺ�������ʽ����:
  			AT+CIPSEND=length		//length ���͵����ݳ���
  			OK
  			>xxxxxxx				//���͵�����
  			busy s...
  			Recv xx bytes
  			SEND OK
  			
  			���length����ʵ�ʷ��͵����ݳ��ȣ���˴η��Ͳ�������
  			���lengthС��ʵ�ʷ��͵ĳ��ȣ���˴η���length���ȵ����ݣ���������ݽض϶�ʧ
  * @param:	p��������ָ��
  * @retval:	�ɹ�����1��ʧ��0
****************************************************************************************
u8 client_send_to_server( u8 *p)
{
	u8 temp[6];
	u8 cmd[30]="AT+CIPSEND=";
	u8 *str;
	u16 send_len = strlen(p);
	str = int_to_str(temp, send_len);
	strcat(cmd, str);					//��ϳ���
	strcat(cmd, "\r\n");

	if( !wifi_send_and_wait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))		//���ͷ�������
		return FAIL;
	if( !wifi_send_and_wait(p, "SEND OK", WIFI_SEND_TIMEOUT_MS))			//��������
		return FAIL;

	return SUCCESS;
}

****************************************************************************************
  * @brief:	esp8266��Ϊ����˵�http response �������յ�http�����Ժ�
  			���øú�������response (�ο�HTTP Э��)
  			�ֳ�3���ַ��ͣ��ȷ���http ͷ��Ȼ����content_length�����content
  * @param:	client_id�����ӵĿͻ���id�� p��������ָ��
  * @retval:	�ɹ�����1��ʧ��0
*****************************************************************************************
u8 WIFI_Server_HTTP_Response(u8 *client_id, u8 *content)
{
	u8 http_content_lenth_string[10];
	u8 temp[6];
	u8 *p;
	u16 content_size = strlen(content);
	
	memset(http_content_lenth_string, 0, sizeof(http_content_lenth_string));
	if(!server_send_to_client(client_id, HTTP_Server_Response_Head1) )		//����http ͷ��������content_length
		return FAIL;
	
	p = int_to_str( temp, content_size);			//����content_length���������з�
	strcat(http_content_lenth_string, p);
	strcat(http_content_lenth_string,"\r\n\r\n");
	if(!server_send_to_client(client_id, http_content_lenth_string))
		return FAIL;
	
	if(!server_send_to_client(client_id, content))		//��������
		return FAIL;
	return SUCCESS;

}
*/
/****************************************************************************************
  * @brief:	esp8266��Ϊ�ͻ��������ӵķ���˷��ͺ�������ʽ����:
  			AT+CIPSEND=length		//length ���͵����ݳ���
  			OK
  			>xxxxxxx				//���͵�����
  			busy s...
  			Recv xx bytes
  			SEND OK
  			
  			���length����ʵ�ʷ��͵����ݳ��ȣ���˴η��Ͳ�������
  			���lengthС��ʵ�ʷ��͵ĳ��ȣ���˴η���length���ȵ����ݣ���������ݽض϶�ʧ
  * @param:	p��������ָ��
  * @retval:	�ɹ�����1��ʧ��0
****************************************************************************************
u8 client_send_to_server( u8 *p)
{
	u8 temp[6];
	u8 cmd[30]="AT+CIPSEND=";
	u8 *str;
	u16 send_len = strlen(p);
	str = int_to_str(temp, send_len);
	strcat(cmd, str);					//��ϳ���
	strcat(cmd, "\r\n");

	if( !wifi_send_and_wait(cmd, "> ", WIFI_CMD_TIMEOUT_MS ))		//���ͷ�������
		return FAIL;
	if( !wifi_send_and_wait(p, "SEND OK", WIFI_SEND_TIMEOUT_MS))			//��������
		return FAIL;

	return SUCCESS;
}
*/
