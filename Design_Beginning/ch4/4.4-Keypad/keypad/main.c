/********************************************************************
 
 파  일 :	main.c
 
 동  작 :
	버튼을 눌러 입력 받은 값에 해당하는 LED 점등

 포트연결 :
	IOT SENSOR 모듈의 포트 J12(PORT C)를 LED 모듈의 포트 RED에 연결
	IOT SENSOR 모듈의 포트 J14(PORT K)를 Keypad 모듈의 포트 BT1에 연결
	IOT SENSOR 모듈의 +3.3V와 GND 핀을 Keypad 모듈의 J3 전원 컨넥터에 연결
	(화살표로 표시된 핀이 전원이며, 다른 핀이 GND)
	IOT SENSOR 모듈의 +3.3V와 GND 핀을 LED 모듈의 J3 전원 컨넥터에 연결
	
 소스설명 :
 1) 입출력 관련 헤더파일을 선언
 2) Delay 관련 헤더파일을 선언
 3) GPIO 관련 헤더파일을 선언
 4) KEYPAD와 LED의 개수 정의
 5) LED 및 KEYPAD 포트 정의
 6) 함수 프로토타입 선언
 7) Keypad를 눌렀을 때, 해당 포트의 핀으로 
    신호가 입력되는지 여부를 리턴하는 함수
 8) PORT C를 출력 포트로 사용
 9) LOW(0) 신호 출력
 10) PORT K를 입력 포트로 사용
 11) 앞서 readKeypad 함수를 통해 리턴된 Keypad 상태를 확인하여,
     매칭되는 LED 핀으로 HIGH(1)/LOW(0) 신호를 출력
 
********************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "gpio.h"

#define MAX_KEY_BT_NUM	8
#define MAX_LED_NUM		8

#define LED_PORT	C
#define KEYPAD_PORT	K

int readKeypad(int port);

// port = 
//	A (PORTA), B (PORTB), C (PORTC), D (PORTD), E (PORTE), F (PORTF), 
//	G (PORTG), H (PORTH), J (PORTJ), K (PORTK), L (PORTL),
// return : keypad status
int readKeypad(int port)
{
	int i;
	int nKeypadStatus = 0;
	
	for(i =0; i<MAX_KEY_BT_NUM; i++)
	{
		if(!digitalRead(port, i))
		{
			nKeypadStatus |= (1<<i);
		}
	}
	return nKeypadStatus;
}

void main (void)
{
	int i = 0;
	int nKeypadStatus = 0;
	
	for(i=0; i<MAX_LED_NUM; i++)
	{
		pinMode(LED_PORT, i, OUTPUT);
		digitalWrite(LED_PORT, i, LOW);
	}
	for(i=0; i<MAX_KEY_BT_NUM; i++)
	{
		pinMode(KEYPAD_PORT, i, INPUT);
	}
	
	while(1)
	{
		nKeypadStatus = readKeypad(KEYPAD_PORT);
		
		
		for(i=0; i<MAX_KEY_BT_NUM; i++)
		{
			if((nKeypadStatus & (1<<i)))
			{
				digitalWrite(LED_PORT, i, HIGH);
			}
			else
			{
				digitalWrite(LED_PORT, i, LOW);
			}
		}
	}
}
