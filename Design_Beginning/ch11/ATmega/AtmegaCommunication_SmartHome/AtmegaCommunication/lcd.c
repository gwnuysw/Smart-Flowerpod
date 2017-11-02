
#include "lcd.h"
#include "gpio.h"

void write_Data ( unsigned char data )
{
	int i = 0;

	for(i=0; i<LCD_CD_PIN_NUM; i++)
	{
		digitalWrite(LCD_CD_PORT, i, LOW);		/* RS = 0, R/W = 0, E = 0 */
	}
	
	digitalWrite(LCD_CD_PORT, RS_PIN, HIGH);	/* RS = 1, DR->DDRAM */
	
	//	LCD_DB_PORT	=	data;
	for(i=0; i<LCD_DB_PIN_NUM; i++)
	{
		if((data & (1<<i)))
		{
			digitalWrite(LCD_DB_PORT, i, HIGH);
		}
		else
		{
			digitalWrite(LCD_DB_PORT, i, LOW);
		}
	}	

	digitalWrite(LCD_CD_PORT, E_PIN, HIGH);		/* E = 1 */
	_delay_us(110);				/* Essential Delay for Simulator */
	digitalWrite(LCD_CD_PORT, E_PIN, LOW);		/* E = 0 */
	_delay_us(110);
}

void printString ( char *string )
{
	while ( *string != '\0' )
	{
		write_Data ( *string );
		string ++;
	}
}

void write_Command ( unsigned char command )
{
	int i = 0;
	
	for(i=0; i<LCD_CD_PIN_NUM; i++)
	{
		digitalWrite(LCD_CD_PORT, i, LOW);		/* E = 0, R/W = 0, RS = 0 */
	}
	
	//	LCD_DB_PORT	=	command;
	for(i=0; i<LCD_DB_PIN_NUM; i++)
	{
		if((command & (1<<i)))
		{
			digitalWrite(LCD_DB_PORT, i, HIGH);
		}
		else
		{
			digitalWrite(LCD_DB_PORT, i, LOW);
		}
	}	

	digitalWrite(LCD_CD_PORT, E_PIN, HIGH);		/* E = 1 */
	_delay_us(110);				/* Essential Delay for Simulator */
	digitalWrite(LCD_CD_PORT, E_PIN, LOW);		/* E = 0 */
	_delay_us(110);
}

void LCD_Init (int controlPort, int rsPin, int rwPin, int ePin, int dataPort)
{
	int i = 0;
	
	for(i=0; i<LCD_DB_PIN_NUM; i++)
	{
		pinMode(dataPort, i, OUTPUT);
		digitalWrite(dataPort, i, LOW);
	}

	// 신호선 3 PIN  출력설정
	pinMode(controlPort, rsPin, OUTPUT);
	digitalWrite(controlPort, rsPin, LOW);	
	pinMode(controlPort, rwPin, OUTPUT);
	digitalWrite(controlPort, rwPin, LOW);	
	pinMode(controlPort, ePin, OUTPUT);
	digitalWrite(controlPort, ePin, LOW);

	_delay_us(110);
	digitalWrite(controlPort, ePin, LOW);	/* E = 0 */
	_delay_us(110);

	write_Command(0x38);		/* Function set */
	_delay_us(220);

	write_Command(0x0F);		/* Display ON */
	_delay_us(220);

	write_Command(0x02);		/* Cursor At Home */
	_delay_ms(9);

	write_Command(0x01);		/* Clear Display */
	_delay_ms(9);

	write_Command(0x06);		/* Entry mode set */
	_delay_us(220);
}