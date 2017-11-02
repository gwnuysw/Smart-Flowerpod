/*
 * GasSensor.c
 *
 * Created: 2016-07-21 오전 9:24:29
 * Author : com
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"
#include "gpio.h"
#include "debug.h"

#define LED_PORT				F		//GPIO PORTC
#define LED_PIN					1		//LED Pin

#define GAS_DETECT_LEVEL		500
#define GAS_MODULE_CHANNEL		0

int main(void)
{
	debugInit();
	
	debugprint("Gas Sensor\r\n");
	
	pinMode(LED_PORT,LED_PIN,OUTPUT);
	AdcInit(GAS_MODULE_CHANNEL);
	
	unsigned int unAdcValue;
    while (1) 
    {
		unAdcValue = AdcRead();
		debugprint("unAdcValue : %d\r\n", unAdcValue);
		
		_delay_ms(100);
		if(unAdcValue>GAS_DETECT_LEVEL)
		{
			digitalWrite(LED_PORT,LED_PIN,HIGH);			//LED ON
		}
		else
		{
			digitalWrite(LED_PORT,LED_PIN,LOW);				//LED OFF
		}
    }
	return 0;
}

