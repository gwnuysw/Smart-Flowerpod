/*
 * Led_Module.c
 *
 * Created: 2016-07-20 오전 10:12:32
 * Author : com
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "gpio.h"

#define LED_PORT				F		//GPIO PORTC
#define LED_PIN					0		//LED Pin

int main(void)
{
	
	pinMode(LED_PORT,LED_PIN,OUTPUT);
	
    while (1) 
    {
		digitalWrite(LED_PORT,LED_PIN,HIGH);			//LED ON
		_delay_ms(1000);								//delay 1s
		digitalWrite(LED_PORT,LED_PIN,LOW);				//LED OFF
		_delay_ms(1000);								//delay 1s
    }
	return 0;
}

