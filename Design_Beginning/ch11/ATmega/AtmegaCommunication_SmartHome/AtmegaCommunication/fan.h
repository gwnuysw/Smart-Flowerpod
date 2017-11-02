#ifndef	__FAN_H__
#define	__FAN_H__

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define	FAN_PORT	PORTC
#define	FAN_DDR		DDRC

#define ON	1
#define OFF	0

void initFan(void);
void FAN_Action (unsigned int motion);


#endif	/* __FAN_H__ */