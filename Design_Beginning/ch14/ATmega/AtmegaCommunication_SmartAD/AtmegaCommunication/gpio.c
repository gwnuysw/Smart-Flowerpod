#include "gpio.h"

/***** ATmega2560 *****/

// GPIO Mode Function
// port = 
//	A (PORTA), B (PORTB), C (PORTC), D (PORTD), E (PORTE), F (PORTF), 
//	G (PORTG), H (PORTH), J (PORTJ), K (PORTK), L (PORTL),
// pin  = 0~7
// mode = 0(Input), 1(OutPut)
void pinMode(uint8_t port,uint8_t pin,uint8_t mode)
{
	if(port == A) {
		if(mode) {
			sbit(DDRA,pin);	
		}
		else {
			cbit(DDRA,pin);
		}	
	}
	else if(port == B) {
		if(mode) {
			sbit(DDRB,pin);	
		}
		else {
			cbit(DDRB,pin);
		}	
	}
	else if(port == C) {
		if(mode) {
			sbit(DDRC,pin);	
		}
		else {
			cbit(DDRC,pin);
		}
	}	
	else if(port == D) {
		if(mode) {
			sbit(DDRD,pin);	
		}
		else {
			cbit(DDRD,pin);
		}
	}
	else if(port == E) {
		if(mode) {
			sbit(DDRE,pin);	
		}
		else {
			cbit(DDRE,pin);
		}
	}
	else if(port == F) {
		if(mode) {
			sbit(DDRF,pin);	
		}
		else {
			cbit(DDRF,pin);
		}
	}
	else if(port == G) {
		if(mode) {
			sbit(DDRG,pin);	
		}
		else {
			cbit(DDRG,pin);
		}
	}
	else if(port == H) {
		if(mode) {
			sbit(DDRH,pin);	
		}
		else {
			cbit(DDRH,pin);
		}
	}
	// PORTI is not present
	else if(port == J) {
		if(mode) {
			sbit(DDRJ,pin);	
		}
		else {
			cbit(DDRJ,pin);
		}
	}
	else if(port == K) {
		if(mode) {
			sbit(DDRK,pin);
		}
		else {
			cbit(DDRK,pin);
		}
	}
	else if(port == L) {
		if(mode) {
			sbit(DDRL,pin);
		}
		else {
			cbit(DDRL,pin);
		}
	}
}

// GPIO Write Function
// port = 
//	A (PORTA), B (PORTB), C (PORTC), D (PORTD), E (PORTE), F (PORTF), 
//	G (PORTG), H (PORTH), J (PORTJ), K (PORTK), L (PORTL),
// pin  = 0~7
// signal = 0(LOW), 1(HIGH)
void digitalWrite(uint8_t port, uint8_t pin, uint8_t signal)
{
	if(port == A) {
		if(signal) {
			sbit(PORTA,pin);
		}
		else {
			cbit(PORTA,pin);
		}
	}
	else if(port == B) {
		if(signal) {
			sbit(PORTB,pin);
		}
		else {
			cbit(PORTB,pin);
		}
	}
	else if(port == C) {
		if(signal) {
			sbit(PORTC,pin);
		}
		else {
			cbit(PORTC,pin);
		}
	}
	else if(port == D) {
		if(signal) {
			sbit(PORTD,pin);
		}
		else {
			cbit(PORTD,pin);
		}
	}
	else if(port == E) {
		if(signal) {
			sbit(PORTE,pin);
		}
		else {
			cbit(PORTE,pin);
		}
	}
	else if(port == F) {
		if(signal) {
			sbit(PORTF,pin);
		}
		else {
			cbit(PORTF,pin);
		}
	}
	else if(port == G) {
		if(signal) {
			sbit(PORTG,pin);
		}
		else {
			cbit(PORTG,pin);
		}
	}
	else if(port == H) {
		if(signal) {
			sbit(PORTH,pin);
		}
		else {
			cbit(PORTH,pin);
		}
	}
	// PORTI is not present
	else if(port == J) {
		if(signal) {
			sbit(PORTJ,pin);
		}
		else {
			cbit(PORTJ,pin);
		}
	}
	else if(port == K) {
		if(signal) {
			sbit(PORTK,pin);
		}
		else {
			cbit(PORTK,pin);
		}
	}
	else if(port == L) {
		if(signal) {
			sbit(PORTL,pin);
		}
		else {
			cbit(PORTL,pin);
		}
	}
}

// GPIO Read Function
// port = 
//	A (PORTA), B (PORTB), C (PORTC), D (PORTD), E (PORTE), F (PORTF), 
//	G (PORTG), H (PORTH), J (PORTJ), K (PORTK), L (PORTL),
// pin  = 0~7
int digitalRead(uint8_t port, uint8_t pin)
{
	unsigned char ucReadValue;
	if(port == A)
	{
		ucReadValue = (PINA>>pin)&0x01;
	}
	else if(port == B)
	{
		ucReadValue = (PINB>>pin)&0x01;
	}
	else if(port == C)
	{
		ucReadValue = (PINC>>pin)&0x01;
	}
	else if(port == D)
	{
		ucReadValue = (PIND>pin)&0x01;
	}
	else if(port == E)
	{
		ucReadValue = (PINE>>pin)&0x01;
	}
	else if(port == F)
	{
		ucReadValue = (PINF>>pin)&0x01;
	}
	else if(port == G)
	{
		ucReadValue = (PING>>pin)&0x01;
	}
	else if(port == H)
	{
		ucReadValue = (PINH>>pin)&0x01;
	}
	// PORTI is not present
	else if(port == J)
	{
		ucReadValue = (PINJ>>pin)&0x01;
	}
	else if(port == K)
	{
		ucReadValue = (PINK>>pin)&0x01;
	}
	else if(port == L)
	{
		ucReadValue = (PINL>>pin)&0x01;
	}
	
	return ucReadValue;
}

