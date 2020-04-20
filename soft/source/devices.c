#include "usart.h"
#include "devices.h"
#include "string.h"
#include "bsp.h"
#include	"PCA.h"
#include<intrins.h>

#define KEYWORD_SWITCH	"SWITCH"
#define KEYWORD_LIGHT	"LIGHT"

u8 xdata rec_dat[20];   //������ʾ�Ľ�����������

void DHT11_Start(void)
{   	
	DH_Pin=1;   
	delay_10us(1);
	DH_Pin=0;   
	delay_ms(18);   //��ʱ18ms����   
	DH_Pin=1;   
	delay_10us(4);
}

u8 DHT11_rec_byte(void)      //����һ���ֽ�
{   
	u8 i,dat=0;  
	for(i=0;i<8;i++)    //�Ӹߵ������ν���8λ����   
	{                
		while(!DH_Pin);   ////�ȴ�50us�͵�ƽ��ȥ      ����ʱ���������
		delay_10us(2);
		dat<<=1;           //��λʹ��ȷ����8λ���ݣ�����Ϊ0ʱֱ����λ      
		if(DH_Pin==1)    //����Ϊ1ʱ��ʹdat��1����������1         
		dat+=1;      
		while(DH_Pin);  //�ȴ�����������        
	}     
	return dat;
}

void DHT11_Receive()      //����40λ������
{    
	u8 RH=0,RL=0,TH=0,TL=0,check=0;    
	DHT11_Start();    
	if(DH_Pin==0)    
	{        
		while(DH_Pin==0);   //�ȴ�����       
		EA=0;
		delay_10us(8);  //���ߺ���ʱ80us        
		RH=DHT11_rec_byte();    //����ʪ�ȸ߰�λ         
		RL=DHT11_rec_byte();    //����ʪ�ȵͰ�λ          
		TH=DHT11_rec_byte();    //�����¶ȸ߰�λ         
		TL=DHT11_rec_byte();    //�����¶ȵͰ�λ       
		check=DHT11_rec_byte(); //����У��λ        
		EA=1;
		if((RH+RL+TH+TL)==check)      //У��        
			debug("DHT11 true  \r\n");
		else
			debug("DHT11 false  \r\n");
		/*���ݴ���������ʾ*/        
		rec_dat[0]='0'+(RH/10);        
		rec_dat[1]='0'+(RH%10);       
		rec_dat[2]='0'+(RL/10);        
		rec_dat[3]='0'+(RL%10);       
		rec_dat[4]='R';        
		rec_dat[5]='H';        
		rec_dat[6]=' ';        
		rec_dat[7]=' ';        
		rec_dat[8]='0'+(TH/10);       
		rec_dat[9]='0'+(TH%10);       
		rec_dat[10]='0'+(TL/10);       
		rec_dat[11]='0'+(TL%10);        
		rec_dat[12]='C';    
		rec_dat[13]='\0';    
	}
}

static void SwitchControl(u8 *buf)
{
	u8 *cmd=buf;
	u8 *p;
	if( p=strstr(cmd, KEYWORD_SWITCH))
	{
		switch(*(p+sizeof(KEYWORD_SWITCH)))
		{
			case '1':
				if(strstr((char *)cmd, "ON"))
				{
					SWITCH1_ON;
					debug("1 on!\r\n");
				}
				else if(strstr((char *)cmd, "OFF"))
				{
					SWITCH1_OFF;
					debug("1 off!\r\n");
				}
				break;
			case '2':
				if(strstr((char *)cmd, "ON"))
				{
					SWITCH2_ON;
					debug("2 on!\r\n");
				}
				else if(strstr((char *)cmd, "OFF"))
				{
					SWITCH2_OFF;
					debug("2 off!\r\n");
				}
				break;
			case '3':
				if(strstr((char *)cmd, "ON"))
				{
					SWITCH3_ON;
					debug("3 on!\r\n");
				}
				else if(strstr((char *)cmd, "OFF"))
				{
					SWITCH3_OFF;
					debug("3 off!\r\n");
				}
				break;
			case '4':
				if(strstr((char *)cmd, "ON"))
				{
					SWITCH4_ON;
					debug("4 on!\r\n");
				}
				else if(strstr((char *)cmd, "OFF"))
				{
					SWITCH4_OFF;
					debug("4 off!\r\n");
				}
				break;
			default:
				debug("invalid command222\r\n");
				break;
		}
	}
	else
	{
		debug("invalid command111\r\n");
	}
}

static void LightControl(u8 *buf)
{
	
}

void DevicesInit(void)
{
	GPIO_InitTypeDef	light_pin_init, switch_pin_init, dh_pin_init;
	PCA_InitTypeDef pca0_init;

	light_pin_init.Mode=GPIO_OUT_PP;
	light_pin_init.Pin=LIGHT1_GPIO_PIN | LIGHT2_GPIO_PIN;
	GPIO_Inilize(LIGHT1_GPIO_PORT, &light_pin_init);
	
	switch_pin_init.Mode=GPIO_OUT_PP;
	switch_pin_init.Pin=SWITCH1_GPIO_PIN|SWITCH2_GPIO_PIN|SWITCH3_GPIO_PIN|SWITCH4_GPIO_PIN;
	GPIO_Inilize(SWITCH1_GPIO_PORT,&switch_pin_init);

	dh_pin_init.Mode=GPIO_PullUp;
	dh_pin_init.Pin=DH_GPIO_PIN;
	GPIO_Inilize(DH_GPIO_PORT, &dh_pin_init);

	pca0_init.PCA_IoUse = PCA_P24_P25_P26_P27;
	pca0_init.PCA_Clock = PCA_Clock_12T;
	pca0_init.PCA_Mode = PCA_Mode_PWM;
	pca0_init.PCA_PWM_Wide = PCA_PWM_8bit;
	pca0_init.PCA_Interrupt_Mode = DISABLE;
	pca0_init.PCA_Polity = PolityHigh;
	pca0_init.PCA_Value = 0;

	PCA_Init(PCA_Counter, &pca0_init);
	PCA_Init(PCA0, &pca0_init);
	CR=1;
}

void DevicesControl(void)
{

	DevicesInit();
	while(1)
	{
		DHT11_Receive();
		debug(rec_dat);
		delay_s(1);
		
	}
}

