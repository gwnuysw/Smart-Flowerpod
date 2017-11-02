/********************************************************************

 파  일 : main.c
 
 동  작 : 
 	프로그램이 시작하면 500ms 간격으로 LED1 한개 램프를 ON/OFF

 결  선 :
	IOT SENSOR MODULE의 포트 J12(PORT C)의 핀 1(PC0)을 LED 모듈의 포트 RED의 핀 1에 연결 
	IOT SENSOR MODULE의 핀 +3.3V를 LED 모듈의 포트 J3의 우측 핀에 연결
	IOT SENSOR MODULE의 핀 GND를 LED 모듈의 포트 J3의 좌측 핀에 연결

 소스 설명 :
 1) 입출력 관련 헤더파일을 선언
 2) Delay 관련 헤더파일을 선언
 3) GPIO 관련 헤더파일을 선언
 4) PORT C의 PC0를 출력으로 설정
 5) PORT C의 PC0에 LOW(0) 출력
 6) PORT C의 PC0에 HIGH(1) 출력
 7) 500ms 지연
 8) PORT C의 PC0에 LOW(0) 출력
 9) 500ms 지연 

********************************************************************/

#include <avr/io.h>
#include <util/delay.h>

#include "gpio.h"

void main (void)
{
	pinMode(C, 0, OUTPUT);
	digitalWrite(C, 0, LOW);
	while(1)
	{
		digitalWrite(C, 0, HIGH);
		_delay_ms(500);
		digitalWrite(C, 0, LOW);
		_delay_ms(500);
	}
}