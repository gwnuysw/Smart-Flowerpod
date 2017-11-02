
#include "led.h"
#include "gpio.h"

void LED_Init (int ledPort, int ledPin)
{
	pinMode(ledPort, ledPin, OUTPUT);		//LED PORT Set
	digitalWrite(ledPort, ledPin, LOW);		//LED OFF
}

// ledStatus : HIGH or LOW
void controlLED (int ledPort, int ledPin, int ledStatus)
{
	if(ledStatus)
	{
		digitalWrite(ledPort,ledPin,HIGH);		//LED ON
	}
	else
	{
		digitalWrite(ledPort,ledPin,LOW);		//LED OFF
	}
}
