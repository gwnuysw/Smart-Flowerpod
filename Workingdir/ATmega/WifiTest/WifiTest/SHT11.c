#include "sensor.h"
#include "gpio.h"

volatile unsigned short temp;
volatile unsigned short humi;
void printValue (unsigned char sense, unsigned short senseVal)
{
	unsigned char tens, units, tenths;
	if (sense == TEMP)
 	//	printString (" Temp: ");
 	else if (sense == HUMI)
	// 	printString (" Humi: "); 
 	tens		=	senseVal / 100;		
 	units		=	senseVal % 100 / 10;	
	tenths	=	senseVal % 100 % 10;		 
 	if (tens > 0)
 	//	write_Data (tens+'0');
 	else
 	//	printString (" "); 
// 	write_Data ((units)+'0');
//	printString (".");				
//	write_Data ((tenths)+'0');			write_Data함수와 write_Command printString은 LCD함수다. 
}

void main (void)
{
	SHT11_Init();

	_delay_ms(9);	
	while (1)
	{
		temp = get_SHT11_data (TEMP);	// Sensing Temp
		_delay_ms (100);
		humi = get_SHT11_data (HUMI); 	// Sensing Humi
		_delay_ms (100);
//		write_Command (0x02);		/* Cursor At Home, 84ms */
		_delay_ms (9);			
//		printValue (TEMP, temp);			
//		write_Command (0xC0);		/* Move into 2nd Line */
		_delay_us (220);			
//		printValue (HUMI, humi);			
		_delay_ms (300);
	}
}
