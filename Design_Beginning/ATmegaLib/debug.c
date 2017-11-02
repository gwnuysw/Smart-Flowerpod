#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include "debug.h"

#define MAX_DEBUG_RX_DATA	64
volatile unsigned char g_debugRxData[MAX_DEBUG_RX_DATA]; // Rx buff
volatile unsigned char g_debugRxCurrentIndex = 0;       // Rx buffer index

void putCh2(char ch)
{
	while (!(UCSR2A & 0x20));
		UDR2= ch;
}

ISR(USART2_RX_vect)
{
	g_debugRxData[g_debugRxCurrentIndex] = UDR2;
	g_debugRxCurrentIndex++;
	if(g_debugRxCurrentIndex >= MAX_DEBUG_RX_DATA)
		g_debugRxCurrentIndex = 0;
}

#define BAUD_RATE 115200L
void debugInit()
{
	// USART2 use
	// PH0 => RX, PH1 => TX
	DDRH  &= 0xFC;										// "0" input, "1" output
	PORTH |= 0x00;
	
	// Not Double mode, Not multi_communication
	UCSR2A = 0x00;								// x2 Mode
	UCSR2B = 0x98;
	UCSR2C = 0x06;
	
	unsigned int baud=1;   									// ??? baud???? "1"?? ????
	baud = ((F_CPU+(BAUD_RATE*8L))/(BAUD_RATE*16L)-1);      // baud?? ???ес?????? ????

	//	UBRR0H = 0x00;	// (FOSC / (16L * Baudrate)) - 1
	//	UBRR0L = 0x07;	// 0b00110011, (14745600 / (16 * 115200)) - 1 = 0x07
	//	UBRR0L = 0x5F;	// 0b00110011, (14745600 / (16 * 9600)) - 1 = 0x5F
	//	UBRR0L = 0x3F;	// 0b00110011, (14745600 / (16 * 14400)) - 1 = 0x40
	//	UBRR0L = 0x2F;	// 0b00110011, (14745600 / (16 * 19200)) - 1 = 0x2F
	//	UBRR0L = 0x17;	// 0b00110011, (14745600 / (16 * 38400)) - 1 = 0x17
	//	UBRR0L = 0x0F;	// 0b00110011, (14745600 / (16 * 57600)) - 1 = 0x0F

	UBRR2H=(unsigned char) (baud >>8);                            // baud ?????? ??????  H???????? ????
	UBRR2L=(unsigned char) (baud & 0xFF);                         // baud ?????? ??????  L???????  ????
	
	//	UBRR2H = 0x00;
	//	UBRR2L = 16;								// 115200
}

void debugprint(const char *fmt,...)
{
	va_list ap;
	char s[128];
	char *p_char = s;
	va_start(ap, fmt);
	vsprintf(s, fmt,ap);
	va_end(ap);
	while(0 != *p_char){
		putCh2(*p_char++);
	}	
}

void debugConsoleMain()
{
	int i;

	if ( !g_debugRxCurrentIndex ) 
		return;
	
	for(i = 0; i <g_debugRxCurrentIndex;i++)
	{

	}
	g_debugRxCurrentIndex = 0;
}
