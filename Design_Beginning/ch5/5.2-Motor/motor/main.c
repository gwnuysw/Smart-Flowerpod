/********************************************************************

 파  일 : main.c
 
 동  작 : 
 	3초 간격으로 Motor를 시계/반시계 방향으로 ON/OFF

 포트연결 :
	IOT SENSOR 모듈의 포트 J12(PORT C)의 핀 1(PC0)와 2(PC1)를 
	Motor 모듈의 포트 MOTOR의 핀 MT_P와 MT_N에 연결
	IOT SENSOR 모듈의 +3.3V와 GND 핀을 Motor 모듈의 J3 전원 컨넥터에 연결
	(화살표로 표시된 핀이 전원이며, 다른 핀이 GND)
	
	
 소스 설명 :
 1) 입출력 관련 헤더파일을 선언
 2) Delay 관련 헤더파일을 선언
 3) GPIO 관련 헤더파일을 선언
 4) MOTOR 포트 핀 개수 정의
 5) MOTOR 포트 정의
 6) MOTOR 핀 정의
 7) ON/OFF 정의
 8) CLOCKWISE/COUNTERCLOCKWISE(시계방향/반시계방향) 회전 상수 정의
 9) 모터 제어 관련 함수 프로토타입 정의
 10) motion 및 direction 변수를 받아 Motor을 ON/OFF하는 함수
 11) motion==ON, direction==CLOCKWISE 일 때, 
	MT_P 핀에 HIGH(1), MT_N 핀에 LOW(0) 신호를 전달
 12) motion==ON, direction==COUNTERCLOCKWISE 일 때, 
	MT_P 핀에 LOW(0), MT_N 핀에 HIGH(1) 신호를 전달
 13) motion==OFF 일 때,
	MT_P 핀에 LOW(0), MT_N 핀에 LOW(0) 신호를 전달
 14) 핀 MT_P와 MT_N을 출력 설정 
 15) 핀 MT_P와 MT_N을 LOW(0) 상태로 초기화
 16) Motor 시계방향 ON
 17) 3sec 지연
 18) Motor OFF
 19) 3sec 지연
 20) Motor 반시계방향 ON
 21) 3sec 지연
 22) Motor OFF
 23) 3sec 지연
 
********************************************************************/


#include <avr/io.h>
#include <util/delay.h>
#include "gpio.h"

#define MAX_MOTOR_PIN_NUM		2

#define MOTOR_PORT		C

#define MT_P	0
#define MT_N	1

#define ON	1
#define OFF	0

#define CLOCKWISE	1			// CW
#define COUNTERCLOCKWISE	0	// CCW

void Motor_Action (int motion, int direction);

void Motor_Action (int motion, int direction)
{
	if (motion == ON)			// turn on
	{
		if (direction == CLOCKWISE)
		{
			digitalWrite(MOTOR_PORT, MT_P, HIGH);
			digitalWrite(MOTOR_PORT, MT_N, LOW);
		}
		else if (direction == COUNTERCLOCKWISE)
		{
			digitalWrite(MOTOR_PORT, MT_P, LOW);
			digitalWrite(MOTOR_PORT, MT_N, HIGH);
		}					
	}		
	else if(motion == OFF) 		// turn off
	{
		digitalWrite(MOTOR_PORT, MT_P, LOW);
		digitalWrite(MOTOR_PORT, MT_N, LOW);
	}		
}

void main (void)
{
	int i = 0;

	for(i=0; i<MAX_MOTOR_PIN_NUM; i++)
	{
		pinMode(MOTOR_PORT, i, OUTPUT);
		digitalWrite(MOTOR_PORT, i, LOW);
	}	
	
	while(1)
	{
		Motor_Action(ON, CLOCKWISE);
		_delay_ms(3000);

		Motor_Action(OFF, CLOCKWISE);
		_delay_ms(3000);
		
		Motor_Action(ON, COUNTERCLOCKWISE);
		_delay_ms(3000);

		Motor_Action(OFF, COUNTERCLOCKWISE);
		_delay_ms(3000);
	}
}