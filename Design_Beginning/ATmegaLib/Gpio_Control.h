

#include <avr/io.h>
#include <avr/interrupt.h>
#ifndef GPIO_CONTROL_H_
#define GPIO_CONTROL_H_

#define sbit(x,y) (x |= (1<<y))
#define cbit(x,y) (x &= ~(1<<y))

#define INPUT	0
#define OUTPUT	1

#define HIGH	1
#define LOW		0

void PinModeSet(uint8_t port,uint8_t pin,uint8_t mode);
void WriteDigital(uint8_t port, uint8_t pin, uint8_t mode);
int ReadDigital(uint8_t port, uint8_t pin);

#endif /* GPIO_CONTROL_H_ */