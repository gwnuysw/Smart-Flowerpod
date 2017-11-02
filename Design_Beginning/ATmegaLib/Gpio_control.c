
#include "Gpio_Control.h"

// GPIO Mode Set Function
// Port = 1 (PORTC) , Port = 2(PORTK)
//pin  = 0~7, Mode = 0(Input), 1(OutPut)
void PinModeSet(uint8_t port,uint8_t pin,uint8_t mode)
{
	DDRC = 0xFF;
	if(port == 1)
	{
		if(mode)
		{
			sbit(DDRC,pin);	
		}
		else
		{
			cbit(DDRC,pin);
		}	
	}
	else if(port == 2)
	{
		if(mode)
		{
			sbit(DDRK,pin);
		}
		else
		{
			cbit(DDRK,pin);
		}
	}	
}

// GPIO Write Function
// Port = 1 (PORTC) , Port = 2(PORTK)
// pin  = 0~7, Mode = 0(LOW), 1(HIGH)
void WriteDigital(uint8_t port, uint8_t pin, uint8_t mode)
{
	if(port == 1)
	{
		if(mode)
		{
			sbit(PORTC,pin);
		}
		else
		{
			cbit(PORTC,pin);
		}
	}
	else if(port == 2)
	{
		if(mode)
		{
			sbit(PORTK,pin);
		}
		else
		{
			cbit(PORTK,pin);
		}
	}
}

// GPIO  Function
// Port = 1 (PORTC) , Port = 2(PORTK)
//pin  = 0~7, Mode = 0(Input), 1(OutPut)
int ReadDigital(uint8_t port, uint8_t pin)
{
	unsigned char ucReadValue;
	if(port == 1)
	{
		ucReadValue = (PINC>>pin)&0x01;
	}
	else if(port == 2)
	{
		ucReadValue = (PINK>>pin)&0x01;
	}
	return ucReadValue;
}

