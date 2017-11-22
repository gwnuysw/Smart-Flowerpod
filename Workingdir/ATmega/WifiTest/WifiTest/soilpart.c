#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "debug.h"
#include "adc.h"
#include "gpio.h"
#include "soilpart.h"

#define MAX_MOTOR_PIN_NUM		2

#define MOTOR_PORT		F

#define MT_P	6
#define MT_N	3

#define ON	1
#define OFF	0

#define CLOCKWISE	1			// CW
#define COUNTERCLOCKWISE	0	// CCW



void Motor_Action (int motion, int direction)
{
	if (motion == ON)			// turn on
	{
		if (direction == CLOCKWISE)
		{
			digitalWrite(MOTOR_PORT, MT_P, HIGH);
			digitalWrite(MOTOR_PORT, MT_N, LOW);
		}
		else if (direction == COUNTERCLOCKWISE)
		{
			digitalWrite(MOTOR_PORT, MT_P, LOW);
			digitalWrite(MOTOR_PORT, MT_N, HIGH);
		}
	}
	else if(motion == OFF) 		// turn off
	{
		digitalWrite(MOTOR_PORT, MT_P, LOW);
		digitalWrite(MOTOR_PORT, MT_N, LOW);
	}
}
