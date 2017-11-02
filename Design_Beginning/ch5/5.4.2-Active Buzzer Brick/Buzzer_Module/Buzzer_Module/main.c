/*
 * Led_Module.c
 *
 * Created: 2016-07-20 오전 10:12:32
 * Author : com
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "gpio.h"

#define BUZZER_PORT				F		//Buzzer PORTC
#define BUZZER_PIN				0		//Buzzer Pin

int main(void)
{
	pinMode(BUZZER_PORT,BUZZER_PIN,OUTPUT);
	
    while (1) 
    {
		digitalWrite(BUZZER_PORT,BUZZER_PIN,HIGH);		//Buzzer MODULE ON
		_delay_ms(1000);								//delay 1s
		digitalWrite(BUZZER_PORT,BUZZER_PIN,LOW);		//Buzzer MODULE OFF
		_delay_ms(1000);								//delay 1s
    }
	return 0;
}

