#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "btHm11.h"
#include "debug.h"

#define MAX_BT_RX_DATA	512

volatile unsigned char g_btRxData[MAX_BT_RX_DATA];	// Rx buff
volatile unsigned char g_btRxCurrentIndex = 0;		// Rx buffer index


void putCh1(char ch)
{
	while (!(UCSR1A & 0x20));
		UDR1= ch;
}

ISR(USART1_RX_vect)
{
	g_btRxData[g_btRxCurrentIndex] = UDR1;
	g_btRxCurrentIndex++;
	if(g_btRxCurrentIndex >= MAX_BT_RX_DATA)
		g_btRxCurrentIndex = 0;
}
/*
void wifiAndBTModuleReset()
{
	// MOD_RESET(PD6) (wifi module and  BT module reset )  => bt + wifi  reset 
	DDRD  &= 0xBF; // input										// "0" input, "1" output	
	PORTD &= 0xBF; // low 
	
	_delay_ms(30);
	DDRD  |= 0x40; // output
	_delay_ms(50);
	DDRD  &= 0xBF; // input	
	PORTD |= 0x40; // high
	
	_delay_ms(2000);
}
*/
// USART1 »ç¿ë , PD2 =>RX , PD3 =>TX

void baud115200()
{
	// Not Double mode, Not multi_communication
	UCSR1A = 0x00;								// x2 Mode
	UCSR1B = 0x98;
	UCSR1C = 0x06;
	UBRR1H = 0x00;
	UBRR1L = 16;								// 115200
	
}
#define BAUD_RATE 9600
void baud9600()
{
	UCSR1A = 0x00;								// x2 Mode
	UCSR1B = 0x98;
	UCSR1C = 0x06;

	
	unsigned int baud=1;   									// ??? baud???? "1"?? ????
	baud = ((F_CPU+(BAUD_RATE*8L))/(BAUD_RATE*16L)-1);      // baud?? ???¥á?????? ????

	//	UBRR0H = 0x00;	// (FOSC / (16L * Baudrate)) - 1
	//	UBRR0L = 0x07;	// 0b00110011, (14745600 / (16 * 115200)) - 1 = 0x07
	//	UBRR0L = 0x5F;	// 0b00110011, (14745600 / (16 * 9600)) - 1 = 0x5F
	//	UBRR0L = 0x3F;	// 0b00110011, (14745600 / (16 * 14400)) - 1 = 0x40
	//	UBRR0L = 0x2F;	// 0b00110011, (14745600 / (16 * 19200)) - 1 = 0x2F
	//	UBRR0L = 0x17;	// 0b00110011, (14745600 / (16 * 38400)) - 1 = 0x17
	//	UBRR0L = 0x0F;	// 0b00110011, (14745600 / (16 * 57600)) - 1 = 0x0F

	UBRR1H=(unsigned char) (baud >>8);                            // baud ?????? ??????  H???????? ????
	UBRR1L=(unsigned char) (baud & 0xFF);									
}

void btInit()
{
	// USART1 use
	DDRD  &= 0xF3;										// "0" input, "1" output
	PORTD |= 0x00;
	
	baud9600();
//	baud115200();
	
	wifiAndBTModuleReset();
	_delay_ms(100);
}

void btprint(const char *fmt,...)
{
	va_list ap;
	char s[128];
	char *p_char = s;
	va_start(ap, fmt);
	vsprintf(s, fmt,ap);
	va_end(ap);
	while(0 != *p_char){
		putCh1(*p_char++);
	}	
}

extern void putCh2(char ch);

#define MAX_MAC_NUM		12
void btReadMac()
{
	int i = 0;
	
	if ( !g_btRxCurrentIndex )
	{
		return;
	}
		
	if(g_btRxCurrentIndex>=20)
	{
		if((g_btRxData[0]==0x4F)&&(g_btRxData[1]==0x4B)&&(g_btRxData[7]==0x3A))
		{
			for(i = 0; i<MAX_MAC_NUM; i++)
			{
				if( (i%2==0)&&(i!=0) )
					debugprint(":");
				gcBtMacNum[i] = g_btRxData[i+8];
				debugprint("%c",gcBtMacNum[i]);	
			}
			debugprint("\r\n\r\n");
			g_btRxCurrentIndex = 0;
		}
		else
		{
			g_btRxCurrentIndex = 0;
		}
	}
}

void btMain()
{
	int i;

	if ( !g_btRxCurrentIndex )
	{
		return;
	}
	
	for(i = 0; i <g_btRxCurrentIndex;i++)
	{
		putCh2(g_btRxData[i]);
		debugprint("[%02X]",g_btRxData[i]);
	}
	
	g_btRxCurrentIndex = 0;
	
}

