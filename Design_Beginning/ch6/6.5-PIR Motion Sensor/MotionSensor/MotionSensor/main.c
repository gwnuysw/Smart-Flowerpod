#include <avr/io.h>
#include <util/delay.h>
#include "gpio.h"

#define LED_PORT	F
#define LED_PIN		0

#define MOTION_PORT	K 
#define MOTION_PIN	0

int main(void)
{
    
	pinMode(LED_PORT,LED_PIN,OUTPUT);			//LED PORT Set
	pinMode(MOTION_PORT,MOTION_PIN,INPUT);		//MOTION PORT SET
	
	int nTemp;
    while (1) 
    {
		nTemp = digitalRead(MOTION_PORT,MOTION_PIN);		//detect
		
		if(nTemp)				//detection
		{
			digitalWrite(LED_PORT,LED_PIN,HIGH);		//LED ON
		}
		else					//non Detection
		{
			digitalWrite(LED_PORT,LED_PIN,LOW);		//LED OFF
		}
    }
	return 0;
}

