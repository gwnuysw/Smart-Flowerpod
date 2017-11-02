/********************************************************************

 파 일 : main.c
 
 동 작 : 
	프로그램이 시작하면 1열에 "Hello MCU World!" 문자열을,
 	2열에 "Welcom IoT" 문자열을 TEXT LCD에 출력함.

 포트연결 :
	IOT SENSOR 모듈의 포트 J14(PORT K)의 핀 0~2(PK0~PK2)를
	Text LCD 모듈의 포트 LCD_CD의 핀 RS/RW/E에 연결
	IOT SENSOR 모듈의 포트 J12(PORT C)를 Text LCD 모듈의 포트 LCD_DB에 연결

 소스 설명 :
 1) LCD 장치 관련 헤더파일 포함
	GPIO 관련 헤더파일 포함
 2) LCD 초기화 함수
 3) Clear Display
 4) Set DDRAM Address
 5) "Hello MCU World!" 표시
 6) Move into 2nd Line
 7) "Welcome AVR" 표시

********************************************************************/

#include "lcd.h"
#include "gpio.h"

void main (void) 
{
	LCD_Init(LCD_CD_PORT, RS_PIN, RW_PIN, E_PIN, LCD_DB_PORT);

	write_Command(0x01);			/* Clear Display */
	_delay_ms(9);		
	write_Command(0x80);			/* Set DDRAM Address */
	_delay_us(220);		
	printString ( "Hello MCU World!" );		
	write_Command(0xC0);			/* Move into 2nd Line */
	_delay_us(220);		
	printString ( "Welcome IoT" );
	_delay_ms(500);
	
	while(1) 
	{
	} 
}
