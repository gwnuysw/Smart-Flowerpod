#include "fan.h"


void initFan(void)
{
	debugprint("Fan Initialize.\n");	
	
	FAN_DDR		|=	0x0C;
	FAN_PORT	&=	~(0x0C);
	
	FAN_Action(OFF);
}

void FAN_Action (unsigned int motion)
{
	if (motion == 1)			// turn on
		FAN_PORT	|=	0x04;
	else						// turn off
		FAN_PORT	&=	~(0x04);
}



