/*****************************************************************************
  * @file:    	devices.c
  * @author:  	louiswoo
  * @version: 	V1.0
  * @date:	2020-4-17
  * @brief:	控制AI  实验板与上层控制设备的交互
协议: 
		设置wifi: 命令"SETWIFI ssid password", 成功返回"OK", 失败返回"FAIL"
		设置switch: 命令"SWITCHx ON/OFF", 成功返回"OK", 失败返回"FAIL"
		设置light: 命令"LIGHTx ON/OFF/INC/DEC", 成功返回"OK", 失败返回"FAIL"
		获取temp: 命令"TEMP", 返回"OK temp"
		获取humi: 命令"HUMI", 返回"OK humi"
*******************************************************************************/

#include "usart.h"
#include "devices.h"
#include "string.h"
#include "bsp.h"
#include	"PCA.h"
#include<intrins.h>

#define KEYWORD_SWITCH	"SWITCH"
#define KEYWORD_LIGHT		"LIGHT"
#define KEYWORD_TEMP		"TEMP"
#define KEYWORD_HUMI		"HUMI"
#define KEYWORD_SETWIFI	"SETWIFI"
#define PCA_LIGHT1	PCA2
#define PCA_LIGHT2	7
#define LIGHT1_ID	PCA2
#define LIGHT2_ID	7

#define LIGHT_POWER_MAX	0xff
#define LIGHT_POWER_MIN	0x00
#define LIGHT_INIT_POWER	160
#define LIGHT_POWER_INC	40
#define LIGHT_POWER_DEC	40

#define LIGHT1_PWM_SET		0xff - Light1Power
#define LIGHT2_PWM_SET		0xff - Light2Power

u8 Light1Power;
u8 Light2Power;

static void DHT11_Start(void)
{   	
	DH_Pin=1;   
	delay_10us(1);
	DH_Pin=0;   
	delay_ms(18);   //延时18ms以上   
	DH_Pin=1;   
	delay_10us(4);
}

static u8 DHT11_rec_byte(void)      //接收一个字节
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

static s16 DHT11_Receive(u8 *cmd)      //接收40位的数据
{    
	u8 RH=0,RL=0,TH=0,TL=0,check=0;    
	u16 temp, humi;
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

		temp = TH;
		humi = RH;

	}
	if(strstr(cmd, KEYWORD_TEMP))
		return temp;
	else if(strstr(cmd, KEYWORD_HUMI))
		return humi;
	return 0xffff;
}

static u8 SwitchControl(u8 *buf)
{
	u8 *cmd=buf;
	debug(">>>>>>>>>>>>>>");
	debug(cmd);
	debug("\r\n");
	switch(*(cmd+sizeof(KEYWORD_SWITCH)-1))
	{
		case '1':
			if(strstr((char *)cmd, "ON"))
			{
				SWITCH1_ON();
				debug("1 on!\r\n");
			}
			else if(strstr((char *)cmd, "OFF"))
			{
				SWITCH1_OFF();
				debug("1 off!\r\n");
			}
			break;
		case '2':
			if(strstr((char *)cmd, "ON"))
			{
				SWITCH2_ON();
				debug("2 on!\r\n");
			}
			else if(strstr((char *)cmd, "OFF"))
			{
				SWITCH2_OFF();
				debug("2 off!\r\n");
			}
			break;
		case '3':
			if(strstr((char *)cmd, "ON"))
			{
				SWITCH3_ON();
				debug("3 on!\r\n");
			}
			else if(strstr((char *)cmd, "OFF"))
			{
				SWITCH3_OFF();
				debug("3 off!\r\n");
			}
			break;
		case '4':
			if(strstr((char *)cmd, "ON"))
			{
				SWITCH4_ON();
				debug("4 on!\r\n");
			}
			else if(strstr((char *)cmd, "OFF"))
			{
				SWITCH4_OFF();
				debug("4 off!\r\n");
			}
			break;
		default:
			debug("switch invalid command\r\n");
			return 0;
			break;
	}
	return 1;
}
static u8 LightOn(u8 light_id)
{
	if (light_id==LIGHT1_ID)
	{
		Light1Power=LIGHT_INIT_POWER;
		PWMn_Update(PCA0, LIGHT1_PWM_SET);
		return 1;
	}
	else if (light_id==LIGHT2_ID)
	{
		Light2Power=LIGHT_INIT_POWER;
		PWMn_Update(PCA1, LIGHT2_PWM_SET);
		return 1;
	}
	else 
		return 0;
}
static u8 LightOff(u8 light_id)
{
	if (light_id==LIGHT1_ID)
	{
		Light1Power=LIGHT_POWER_MIN;
		PWMn_Update(PCA0, LIGHT1_PWM_SET);
		return 1;
	}
	else if (light_id==LIGHT2_ID)
	{
		Light2Power=0;
		PWMn_Update(PCA1, LIGHT2_PWM_SET);
		return 1;
	}
	else 
		return 0;
}

static u8 LightPowerInc(u8 light_id)
{
	if (light_id==LIGHT1_ID)
	{
		if((LIGHT_POWER_MAX - Light1Power) < LIGHT_POWER_INC)
			Light1Power=LIGHT_POWER_MAX;
		else
			Light1Power += LIGHT_POWER_INC;
		PWMn_Update(PCA0, LIGHT1_PWM_SET);
		return 1;
	}
	else if (light_id==LIGHT2_ID)
	{
		if((LIGHT_POWER_MAX - Light2Power) < LIGHT_POWER_INC)
			Light2Power=LIGHT_POWER_MAX;
		else
			Light2Power += LIGHT_POWER_INC;
		PWMn_Update(PCA1, LIGHT2_PWM_SET);
		return 1;
	}
	else 
		return 0;
}
static u8 LightPowerDec(u8 light_id)
{
	if (light_id==LIGHT1_ID)
	{
		if((Light1Power - LIGHT_POWER_MIN)<LIGHT_POWER_DEC)
			Light1Power=LIGHT_POWER_MIN;
		else
			Light1Power -= LIGHT_POWER_DEC;
		PWMn_Update(PCA0, LIGHT1_PWM_SET);
		return 1;
	}
	else if (light_id==LIGHT2_ID)
	{
		if((Light2Power - LIGHT_POWER_MIN)<LIGHT_POWER_DEC)
			Light2Power=LIGHT_POWER_MIN;
		else
			Light2Power -= LIGHT_POWER_DEC;
		PWMn_Update(PCA1, LIGHT2_PWM_SET);
		return 1;
	}
	else 
		return 0;
}

static u8 LightControl(u8 *buf)
{
	u8 *cmd=buf;
	u8 *p;
	if( p=strstr(cmd, KEYWORD_LIGHT))
	{
		switch(*(p+sizeof(KEYWORD_LIGHT)))
		{
			case '1':
				if(strstr((char *)cmd, "ON"))
				{
					LightOn(LIGHT1_ID);
					debug("light1 on!\r\n");
				}
				else if(strstr((char *)cmd, "OFF"))
				{
					LightOff(LIGHT1_ID);
					debug("light1 off!\r\n");
				}
				else if(strstr((char *)cmd, "INC"))
				{
					LightPowerInc(LIGHT1_ID);
					debug("light1 inc!\r\n");
				}
				else if(strstr((char *)cmd, "DEC"))
				{
					LightPowerDec(LIGHT1_ID);
					debug("light1 dec!\r\n");
				}
				break;
			case '2':
				if(strstr((char *)cmd, "ON"))
				{
					LightOn(LIGHT2_ID);
					debug("light1 on!\r\n");
				}
				else if(strstr((char *)cmd, "OFF"))
				{
					LightOff(LIGHT2_ID);
					debug("light1 off!\r\n");
				}
				else if(strstr((char *)cmd, "INC"))
				{
					LightPowerInc(LIGHT2_ID);
					debug("light2 inc!\r\n");
				}
				else if(strstr((char *)cmd, "DEC"))
				{
					LightPowerDec(LIGHT2_ID);
					debug("light2 dec!\r\n");
				}
				break;
			default:
				debug("light invalid command222\r\n");
				break;
		}
	}
	else
	{
		debug("invalid command111\r\n");
	}
	return 1;
}

void DevicesInit(void)
{
	GPIO_InitTypeDef	light_pin_init, switch_pin_init, dh_pin_init;
	PCA_InitTypeDef pca_init;

	switch_pin_init.Mode=GPIO_OUT_PP;
	switch_pin_init.Pin=SWITCH1_GPIO_PIN;
	GPIO_Inilize(SWITCH1_GPIO_PORT,&switch_pin_init);
	switch_pin_init.Pin=SWITCH2_GPIO_PIN|SWITCH3_GPIO_PIN|SWITCH4_GPIO_PIN;
	GPIO_Inilize(SWITCH2_GPIO_PORT,&switch_pin_init);

	dh_pin_init.Mode=GPIO_PullUp;
	dh_pin_init.Pin=DH_GPIO_PIN;
	GPIO_Inilize(DH_GPIO_PORT, &dh_pin_init);

	light_pin_init.Mode=GPIO_OUT_PP;
	light_pin_init.Pin=LIGHT1_GPIO_PIN;
	GPIO_Inilize(LIGHT1_GPIO_PORT, &light_pin_init);
	light_pin_init.Pin=LIGHT2_GPIO_PIN;
	GPIO_Inilize(LIGHT2_GPIO_PORT, &light_pin_init);
	
	pca_init.PCA_IoUse = PCA_P12_P11_P10_P37;
	pca_init.PCA_Clock = PCA_Clock_12T;
	pca_init.PCA_Mode = PCA_Mode_PWM;
	pca_init.PCA_PWM_Wide = PCA_PWM_8bit;
	pca_init.PCA_Interrupt_Mode = DISABLE;
	pca_init.PCA_Polity = PolityHigh;
	pca_init.PCA_Value = 0;

	PCA_Init(PCA_Counter, &pca_init);
	PCA_Init(PCA_LIGHT1, &pca_init);
	PCA_Init(PCA_LIGHT2, &pca_init);

	SWITCH1_OFF();
	SWITCH2_OFF();
	SWITCH3_OFF();
	SWITCH4_OFF();

	LightOff(PCA_LIGHT1);
	LightOff(PCA_LIGHT2);

	Light1Power=0;
	Light2Power=0;
}

u8 DevicesControl(u8 *cmd)
{
	u8 *p;
	p=strstr(cmd, KEYWORD_SWITCH);
	if(p)
	{
		if(SwitchControl(p))
			return 1;
		else
			return 0;
	}
	else if(p=strstr(cmd, KEYWORD_LIGHT))
		LightControl(p);
	else if(p=strstr(cmd, KEYWORD_TEMP))
		DHT11_Receive(p);
	else if(p=strstr(cmd, KEYWORD_HUMI))
		DHT11_Receive(p);
	
	return 1;
}


