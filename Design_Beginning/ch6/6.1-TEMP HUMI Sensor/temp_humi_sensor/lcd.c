/********************************************************************

 파 일 : lcd.c

 소스 설명 :
 1) 데이터를 Write 하는 함수
   - RS = 0, R/W = 0, E = 0
   - RS = 1
   - LCD_DB포트로 데이터 출력
   - E = 1
   - Delay
   - E = 0
 2) 데이터 Write 함수를 통한 문자열 표시
 3) 명령 데이터를 Write 하는 함수
  - RS = 0, R/W = 0, E = 0
  - LCD_DB포트로 명령 데이터 출력
  - E = 1
  - Delay
  - E = 0
 4) LCD 초기화 함수
  - 초기화에 필요한 명령을 통해
    기본적인 설정을 구성하는 부분
  - Function set
  - Display On
  - Cursor At Home
  - Clear Display
  - Entry mode set

********************************************************************/

#include "lcd.h"

void write_Data ( unsigned char data )
{
	int i = 0;
	
//	LCD_CD_PORT	=	0x00;		/* RS = 0, R/W = 0, E = 0 */
//	LCD_CD_PORT	|=	0x01;		/* RS = 1, DR->DDRAM */
//	LCD_DB_PORT	=	data;
	
	for(i=0; i<LCD_CD_PIN_NUM; i++)
	{
		digitalWrite(LCD_CD_PORT, i, LOW);
	}
	
	digitalWrite(LCD_CD_PORT, RS_PIN, HIGH);
	
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

//	LCD_CD_PORT	|=	0x04;		/* E = 1 */
	digitalWrite(LCD_CD_PORT, E_PIN, HIGH);
	_delay_us(110);				/* Essential Delay for Simulator */
//	LCD_CD_PORT	&=	~(0x04);	/* E = 0 */
	digitalWrite(LCD_CD_PORT, E_PIN, LOW);
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
//	LCD_CD_PORT	=	0x00;		/* E = 0, R/W = 0, RS = 0 */
//	LCD_DB_PORT	=	command;	/* Command */
	
	for(i=0; i<LCD_CD_PIN_NUM; i++)
	{
		digitalWrite(LCD_CD_PORT, i, LOW);
	}
	
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

//	LCD_CD_PORT	|=	0x04;		/* E = 1 */
	digitalWrite(LCD_CD_PORT, E_PIN, HIGH);
	_delay_us(110);				/* Essential Delay for Simulator */
//	LCD_CD_PORT	&=	~(0x04);	/* E = 0 */
	digitalWrite(LCD_CD_PORT, E_PIN, LOW);
	_delay_us(110);
}

void LCD_Init (int controlPort, int rsPin, int rwPin, int ePin, int dataPort)
{
	int i = 0;
//	LCD_DB_DDR		=	0xFF;
//	LCD_DB_PORT		=	0x00;
//	LCD_CD_DDR		=	0x07;	// 신호선 3 PIN  출력설정
//	LCD_CD_PORT		=	0x00;
	
	for(i=0; i<LCD_DB_PIN_NUM; i++)
	{
		pinMode(dataPort, i, OUTPUT);
		digitalWrite(dataPort, i, LOW);
	}

	pinMode(controlPort, rsPin, OUTPUT);
	digitalWrite(controlPort, rsPin, LOW);
	
	pinMode(controlPort, rwPin, OUTPUT);
	digitalWrite(controlPort, rwPin, LOW);
	
	pinMode(controlPort, ePin, OUTPUT);
	digitalWrite(controlPort, ePin, LOW);

#if 0
	for(i=0; i<LCD_CD_PIN_NUM; i++)
	{
		pinMode(LCD_CD_PORT, i, OUTPUT);
		digitalWrite(LCD_CD_PORT, i, LOW);
	}
#endif

	_delay_us(110);
//	LCD_CD_PORT	&=	~(0x04);	/* E = 0 */
	digitalWrite(controlPort, ePin, LOW);
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