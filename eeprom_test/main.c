#include "eeprom.h"
#include "usart.h"
#include "stc15fxxxx.h"

#define debug(x)	Usart1SendString(x)
#define debug_byte(x)	Usart1SendByte(x)


//´®¿Ú1³õÊ¼»¯
void Usart1Init(void)		//115200bps@22.1184MHz
{
	SCON = 0x50;		//8???,?????
	AUXR |= 0x40;		//???1???Fosc,?1T
	AUXR &= 0xFE;		//??1?????1???????
	TMOD &= 0x0F;		//?????1?16???????
	TL1 = 0xD0;		//??????
	TH1 = 0xFF;		//??????
	ET1 = 0;		//?????1??
	TR1 = 1;		//?????1
	

	ES =0;

}

void Usart1SendByte(u8 dat)
{
	SBUF = dat;
	while(!TI);
	TI = 0;
}

void Usart1SendString(u8 *str)
{
	while(*str)
	{
		debug_byte(*str);
	}
}
void main(void)
{
	u16 i;
	u8 str[] = {"i am genius !!!!!!!i am genius !!!!!!!i am genius !!!!!!!i am genius !!!!!!!i am genius !!!!!!!"};
	u8 read_buf[200], write_buf[200];
	u8 dat;
	dat = 0x33;
	for(i=0; i<200; i++)
	{
		write_buf[i] = 0x44;
	}
	Usart1Init();
	debug_byte(dat);
	EEPROM_SectorErase(EEPROM_START_ADDRESS);
	EEPROM_read_n(EEPROM_START_ADDRESS, read_buf, 200);
	for(i=0; i < 200; i++)
	{
		debug_byte(read_buf[i]);

	}
	
	EEPROM_write_n(EEPROM_START_ADDRESS, str, sizeof(str));
	EEPROM_read_n(EEPROM_START_ADDRESS, read_buf, 200);
	for(i=0; i < 200; i++)
	{
		debug_byte(read_buf[i]);

	}

	while(1)
	{}
}
