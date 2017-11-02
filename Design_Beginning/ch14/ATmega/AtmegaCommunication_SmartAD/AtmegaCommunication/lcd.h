#ifndef __LCD_H__
#define __LCD_H__

#include <avr/io.h>
#include <util/delay.h>

#define LCD_CD_PORT		K
#define LCD_DB_PORT		C
#define RS_PIN	0
#define RW_PIN	1
#define E_PIN	2

#define LCD_CD_PIN_NUM		3
#define LCD_DB_PIN_NUM		8

void write_Data ( unsigned char data );
void printString ( char *string );
void write_Command ( unsigned char command );
void LCD_Init (int controlPort, int rsPin, int rwPin, int ePin, int dataPort);

#endif	/* __LCD_H__ */