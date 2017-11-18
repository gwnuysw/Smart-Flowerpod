#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//#include "debug.h"
#include "gpio.h"
#include "adc.h"

#define MAX_LED_NUM		8

/* LED PORT Define*/
//#define GREEN_PORT	K
//#define RED_PORT	C

/* PORTF is used by ADC1 */

#define RED		0
#define GREEN	1

extern volatile uint32_t timerCouter;
extern volatile uint32_t setTimerCheckCounter;
extern volatile uint8_t  flagOverflowSetTimer;

//unsigned int gun_AdcValue	=	0;
unsigned char guc_OverflowCnt	=	0;
unsigned int n_flag_getADC = 0;

ISR (TIMER0_OVF_vect)	// Timer0 Overflow0 ISR
{
	cli();																	//----------------v 기능 복붙
	// 0.01s에 오버플로우 발생, 1/(14745600Hz/1024)ⅹ144 = 0.01s
	TCNT0	=	0xff - 144;

	timerCouter++;
	
	if ( timerCouter >= setTimerCheckCounter )
	{
		flagOverflowSetTimer = 1;
		timerCouter = 0;
	}
	sei();
														//-------------------------------------^기능 복붙
	guc_OverflowCnt++;

	if (guc_OverflowCnt == 100)		// 0.003s * 100 = 0.3s
	{
		guc_OverflowCnt	=	0;
		n_flag_getADC = 1;
	}
}
/*-----------------------------led 부분제거*/
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
