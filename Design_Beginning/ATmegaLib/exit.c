#include "exit.h"

// ExitNum : INT0 ~ INT7
// SenseControl : LOW, CHANGE, FALLING, RASING
void ExitInit (int ExitNum, int SenseControl)
{
	unsigned char ucInterruptSenseControl = RISING;		// default setting : rising edge
	
	EIMSK	|=	1<<ExitNum;				// 외부인터럽트 INT0 Enable

	switch(SenseControl)	
	{
		case LOW:
			ucInterruptSenseControl = 0;
			break;
		case CHANGE:
			ucInterruptSenseControl = (1<<ISC00);
			break;
		case FALLING:
			ucInterruptSenseControl = (1<<ISC01);
			break;
		case RISING:
			ucInterruptSenseControl = (1<<ISC01)|(1<<ISC00);
			break;
		default:
			ucInterruptSenseControl = (1<<ISC01)|(1<<ISC00);
			break;
	}

	if(ExitNum<4)
	{
		EICRA	|=	ucInterruptSenseControl<<(ExitNum*2);
	}
	else
	{
		EICRB	|=	ucInterruptSenseControl<<((ExitNum-4)*2);
	}				

	EIFR	|=	1<<ExitNum;
}
