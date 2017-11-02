#ifndef	__MOTOR_H__
#define	__MOTOR_H__

#include <avr/io.h>
#include <util/delay.h>
#include "gpio.h"


#define MAX_MOTOR_PIN_NUM		2

#define MOTOR_PORT		C

#define MT_P	0
#define MT_N	1

#define MOTOR_ON	1
#define MOTOR_OFF	0

#define CLOCKWISE	1			// CW
#define COUNTERCLOCKWISE	0	// CCW

void Motor_Init (void);
void Motor_Action (int motion, int direction);


#endif	/* __MOTOR_H__ */