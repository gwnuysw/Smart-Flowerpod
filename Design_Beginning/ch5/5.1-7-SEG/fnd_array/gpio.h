#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef GPIO_H_
#define GPIO_H_

#define sbit(x,y) (x |= (1<<y))
#define cbit(x,y) (x &= ~(1<<y))

#define INPUT	0
#define OUTPUT	1

#define HIGH	1
#define LOW		0

#define		A		0
#define		B		1
#define		C		2
#define		D		3
#define		E		4
#define		F		5
#define		G		6
#define		H		7
#define		I		8
#define		J		9
#define		K		10
#define		L		11


void pinMode(uint8_t port,uint8_t pin,uint8_t mode);
void digitalWrite(uint8_t port, uint8_t pin, uint8_t signal);
int digitalRead(uint8_t port, uint8_t pin);

#endif /* GPIO_H_ */