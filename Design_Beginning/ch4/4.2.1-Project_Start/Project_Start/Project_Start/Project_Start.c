#include <avr/io.h>
#include <util/delay.h>
#include "gpio.h"

void main (void)
{
	int i = 0;
		
	for(i=0; i<8; i++)
	{
		pinMode(C, i, OUTPUT);
		digitalWrite(C, i, LOW);
	}

	while(1)
	{
		for(i=0; i<8; i++)
		{
			digitalWrite(C, i, HIGH);
			_delay_ms(200);
		}
		for(i=0; i<8; i++)
		{
			digitalWrite(C, i, LOW);
			_delay_ms(200);
		}
	}
}