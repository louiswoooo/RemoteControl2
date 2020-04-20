#include "usart.h"
#include "devices.h"
#include "string.h"
#include "bsp.h"
#include	"PCA.h"
#include<intrins.h>

#define KEYWORD_SWITCH	"SWITCH"
#define KEYWORD_LIGHT	"LIGHT"

u8 xdata rec_dat[20];   //用于显示的接收数据数组

void DHT11_Start(void)
{   	
	DH_Pin=1;   
	delay_10us(1);
	DH_Pin=0;   
	delay_ms(18);   //延时18ms以上   
	DH_Pin=1;   
	delay_10us(4);
}

u8 DHT11_rec_byte(void)      //接收一个字节
{   
	u8 i,dat=0;  
	for(i=0;i<8;i++)    //从高到低依次接收8位数据   
	{                
		while(!DH_Pin);   ////等待50us低电平过去      ，延时函数有误差
		delay_10us(2);
		dat<<=1;           //移位使正确接收8位数据，数据为0时直接移位      
		if(DH_Pin==1)    //数据为1时，使dat加1来接收数据1         
		dat+=1;      
		while(DH_Pin);  //等待数据线拉低        
	}     
	return dat;
}

void DHT11_Receive()      //接收40位的数据
{    
	u8 RH=0,RL=0,TH=0,TL=0,check=0;    
	DHT11_Start();    
	if(DH_Pin==0)    
	{        
		while(DH_Pin==0);   //等待拉高       
		EA=0;
		delay_10us(8);  //拉高后延时80us        
		RH=DHT11_rec_byte();    //接收湿度高八位         
		RL=DHT11_rec_byte();    //接收湿度低八位          
		TH=DHT11_rec_byte();    //接收温度高八位         
		TL=DHT11_rec_byte();    //接收温度低八位       
		check=DHT11_rec_byte(); //接收校正位        
		EA=1;
		if((RH+RL+TH+TL)==check)      //校正        
			debug("DHT11 true  \r\n");
		else
			debug("DHT11 false  \r\n");
		/*数据处理，方便显示*/        
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

