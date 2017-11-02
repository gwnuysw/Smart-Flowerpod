/********************************************************************

 파  일 : main.c
 
 동  작 : 
	Voice Record 모듈의 REC 버튼을 눌러 음성을 녹음
	(FT와 RE-PEAT 스위치는 왼쪽에 위치)
	녹음한 음성이 반복적으로 재생

 포트연결 :
	IOT SENSOR 모듈의 포트 J12(PORT C)에서 핀 1(PC0)을 Voice Record 모듈의 핀 REC에 연결
	IOT SENSOR 모듈의 포트 J12(PORT C)에서 핀 2(PC1)를 Voice Record 모듈의 핀 PLAYE에 연결
	IOT SENSOR 모듈의 포트 J12(PORT C)에서 핀 3(PC2)을 Voice Record 모듈의 핀 PLAYL에 연결
	IOT SENSOR 모듈의 핀 3.3V(+)를 Voice Record 모듈의 핀 VCC에 연결
	IOT SENSOR 모듈의 핀 GND(-)를 Voice Record 모듈의 핀 GND에 연결
	
 소스 설명 :
 1) 입출력 관련 헤더파일을 선언
 2) Delay 관련 헤더파일을 선언
 3) GPIO 관련 헤더파일을 선언
 4) Voice Record 모듈 핀 개수 정의
 5) Voice Record 모듈 포트 및 핀 정의
 6) Voice Record 모듈 초기화 함수
	- Voice Record 모듈 핀 출력 설정
	- Voice Record 모듈 핀 LOW(1)로 초기화
 7) Voice Record 모듈 초기화
 8) 음성 재생을 위하여 Voice Record 모듈 PLAYE 핀 HIGH(1) 설정
	음성 재생을 중단하기 위하여 Voice Record 모듈 PLAYE 핀 LOW(0) 설정

********************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "gpio.h"

#define MAX_RECORD_PIN_NUM		3

#define VOICE_RECORD_PORT	C
#define REC    0
#define PLAYE  1
#define PLAYL  2

void VOICE_RECORD_Init(void)
{
	int i = 0;
	
	for(i=0; i<MAX_RECORD_PIN_NUM; i++)
	{
		pinMode(VOICE_RECORD_PORT, i, OUTPUT);
		digitalWrite(VOICE_RECORD_PORT, i, LOW);
	}	
}	

int main(void)
{
	VOICE_RECORD_Init();
	
    while(1)
    {
		digitalWrite(VOICE_RECORD_PORT, PLAYE, HIGH);
		_delay_ms(5000);
		digitalWrite(VOICE_RECORD_PORT, PLAYE, LOW);
    }
}