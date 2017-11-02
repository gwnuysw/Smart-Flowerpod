#ifndef	__LED_H__
#define	__LED_H__

#include <avr/io.h>
#include <util/delay.h>

#define LED_PORT	F
#define LED_PIN		0

void LED_Init (int ledPort, int ledPin);


#endif	/* __LED_H__ */