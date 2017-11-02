/********************************************************************

 파  일 : main.c
 
 동  작 : 
 	3초 간격으로 FAN이 ON/OFF

 포트연결 :
	AVR 모듈의 포트 J12(PORT C)의 핀 1(PC0)와 2(PC1)를 
	Fan 모듈의 포트 FAN의 핀 MT_P와 MT_N에 연결

	IOT SENSOR 모듈의 포트 J12(PORT C)의 핀 1(PC0)와 2(PC1)를 
	Fan 모듈의 포트 FAN의 핀 MT_P와 MT_N에 연결
	IOT SENSOR 모듈의 +3.3V와 GND 핀을 Fan 모듈의 J3 전원 컨넥터에 연결 
	(화살표로 표시된 핀이 전원이며, 다른 핀이 GND)

 소스 설명 :
 1) 입출력 관련 헤더파일을 선언
 2) Delay 관련 헤더파일을 선언
 3) GPIO 관련 헤더파일을 선언
 4) FAN 포트 핀 개수 정의
 5) FAN 포트 정의
 6) FAN 핀 정의
 7) ON/OFF 정의
 8) Fan 제어 관련 함수 프로토타입 정의
 9) motion 변수를 받아 Fan을 ON/OFF하는 함수
 10) motion==1 일 때, 
	MT_P 핀에 HIGH(1), MT_N 핀에 LOW(0) 신호를 전달
 11) motion==0 일 때,
	MT_P 핀에 LOW(0), MT_N 핀에 LOW(0) 신호를 전달
 12) 핀 MT_P와 MT_N을 출력 설정 
 13) 핀 MT_P와 MT_N을 LOW(0) 상태로 초기화
 14) Fan ON
 15) 3sec 지연
 16) Fan OFF
 17) 3sec 지연
 
********************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "gpio.h"

#define MAX_FAN_PIN_NUM		2

#define FAN_PORT	C

#define MT_P	0
#define MT_N	1

#define ON	1
#define OFF	0

void FAN_Action (unsigned int motion);

void FAN_Action (unsigned int motion)
{
	if (motion == 1)			// turn on
	{
		digitalWrite(FAN_PORT, MT_P, HIGH);
		digitalWrite(FAN_PORT, MT_N, LOW);
	}		
	else						// turn off
	{
		digitalWrite(FAN_PORT, MT_P, LOW);
		digitalWrite(FAN_PORT, MT_N, LOW);
	}		
}

void main (void)
{
	int i = 0;
	
	for(i=0; i<MAX_FAN_PIN_NUM; i++)
	{
		pinMode(FAN_PORT, i, OUTPUT);
		digitalWrite(FAN_PORT, i, LOW);
	}	
	
	while(1)
	{
		FAN_Action(ON);
		_delay_ms(3000);

		FAN_Action(OFF);
		_delay_ms(3000);
	}
}
