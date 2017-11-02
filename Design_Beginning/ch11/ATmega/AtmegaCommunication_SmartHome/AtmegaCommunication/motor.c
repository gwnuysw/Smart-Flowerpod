#include "motor.h"


void Motor_Action (int motion, int direction)
{
	if (motion == MOTOR_ON)			// turn on
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
	else if(motion == MOTOR_OFF) 		// turn off
	{
		digitalWrite(MOTOR_PORT, MT_P, LOW);
		digitalWrite(MOTOR_PORT, MT_N, LOW);
	}		
}

void Motor_Init (void)
{
	int i = 0;

	for(i=0; i<MAX_MOTOR_PIN_NUM; i++)
	{
		pinMode(MOTOR_PORT, i, OUTPUT);
		digitalWrite(MOTOR_PORT, i, LOW);
	}	
}