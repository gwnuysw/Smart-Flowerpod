#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//#include "debug.h"
#include "gpio.h"
#include "adc.h"

#define MAX_LED_NUM		8

/* LED PORT Define*/
#define GREEN_PORT	K
#define RED_PORT	C

/* PORTF is used by ADC1 */

#define RED		0
#define GREEN	1

unsigned int gun_AdcValue	=	0;
unsigned char guc_OverflowCnt	=	0;
int n_flag_getADC = 0;

void TIMER_Init();
void LED_Init ();
void ADC_Init ();
void controlLED (int color, unsigned char data);

ISR (TIMER0_OVF_vect)	// Timer0 Overflow0 ISR
{
	TCNT0	=	0xff - 45;		// 0.003s
	guc_OverflowCnt++;

	if (guc_OverflowCnt == 100)		// 0.003s * 100 = 0.3s
	{
		guc_OverflowCnt	=	0;
		n_flag_getADC = 1;
	}
}

void TIMER_Init (void)
{
	
	TCCR0B	=	0x05;
	
	TCNT0	=	0xff - 45;		// 0.003s
	TIMSK0	|=	1 << TOIE0;		
	TIFR0	|=	1 << TOV0;	// set Overflow Interupt Flag
}

void LED_Init (void)
{
	int i = 0;
	
	for(i=0; i<MAX_LED_NUM; i++)
	{
		pinMode(RED_PORT, i, OUTPUT);
		digitalWrite(RED_PORT, i, LOW);
		pinMode(GREEN_PORT, i, OUTPUT);
		digitalWrite(GREEN_PORT, i, LOW);
	}
}

// color : RED(0), GREEN(1)
void controlLED (int color, unsigned char data)
{
	int i = 0;
	unsigned char nLedStatus = 0;
	nLedStatus	=	data;
	
	for(i=0; i<MAX_LED_NUM; i++)
	{
		if((nLedStatus & (1<<i)))
		{
			if (color == RED)
				digitalWrite(RED_PORT, i, HIGH);
			else if (color == GREEN)
				digitalWrite(GREEN_PORT, i, HIGH);
		}
		else
		{
			if (color == RED)
				digitalWrite(RED_PORT, i, LOW);
			else if (color == GREEN)
				digitalWrite(GREEN_PORT, i, LOW);
		}
	}
}		

void main (void)
{
//	debugInit();
	
	LED_Init ();		// PORTB = GREEN, PORTE = RED
	AdcInit(1);			// PORTF = ADC1
	TIMER_Init();
	
	sei();
	
	while (1)
	{
		if(n_flag_getADC == 1)
		{
			n_flag_getADC = 0;
			
			gun_AdcValue	=	AdcRead();
//			debugprint("gun_AdcValue : %d\r\n", gun_AdcValue);			
		}
		
		
	  	if (gun_AdcValue < 640 && gun_AdcValue >= 600)
		{	controlLED (GREEN, 0x80);	controlLED (RED, 0x00);	}
  		else if (gun_AdcValue < 600 && gun_AdcValue >= 560)
		{	controlLED (GREEN, 0xC0);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 560 && gun_AdcValue >= 520)
		{	controlLED (GREEN, 0xE0);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 520 && gun_AdcValue >= 480)
		{	controlLED (GREEN, 0xF0);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 480 && gun_AdcValue >= 440)
		{	controlLED (GREEN, 0xF8);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 440 && gun_AdcValue >= 400)
		{	controlLED (GREEN, 0xFC);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 400 && gun_AdcValue >= 360)
		{	controlLED (GREEN, 0xFE);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 360 && gun_AdcValue >= 320)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 320 && gun_AdcValue >= 280)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0x80);	}
		else if (gun_AdcValue < 280 && gun_AdcValue >= 240)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xC0);	}
		else if (gun_AdcValue < 240 && gun_AdcValue >= 200)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xE0);	}
		else if (gun_AdcValue < 200 && gun_AdcValue >= 160)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xF0);	}
		else if (gun_AdcValue < 160 && gun_AdcValue >= 120)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xF8);	}
		else if (gun_AdcValue < 120 && gun_AdcValue >= 80)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xFC);	}
		else if (gun_AdcValue < 80 && gun_AdcValue >= 40)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xFE);	}
		else if (gun_AdcValue < 40 && gun_AdcValue >= 0)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xFF);	}
		else
		{	controlLED (GREEN, 0x00);	controlLED (RED, 0x00);	}
	}
}
