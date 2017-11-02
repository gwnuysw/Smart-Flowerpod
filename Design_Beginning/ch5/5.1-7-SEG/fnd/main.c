/********************************************************************

 파 일 : main.c
		 
 동 작 : 
	하나의 FND에 숫자 5를 디스플레이

 포트연결 :
	IOT SENSOR 모듈의 포트 J14(PORT K)의 핀 1(PK0)을 7-SEG 모듈의 포트 FND_SEL의 핀 S0에 연결
	IOT SENSOR 모듈의 포트 J12(PORT C)를 7-SEG 모듈의 포트 FND_DB에 연결
	IOT SENSOR 모듈의 +3.3V와 GND 핀을 7-SEG 모듈의 J3 전원 컨넥터에 연결
	(화살표로 표시된 핀이 전원이며, 다른 핀이 GND)

 소스 설명 :
 1) 입출력 관련 헤더파일을 선언
 2) GPIO 관련 헤더파일을 선언
 3) FND_SEL과 FND_DB 핀 개수 정의
 4) FND_SEL 및 FND_DB 포트 정의
 5) FND 표시 숫자 "5" 데이터 값 정의
 6) PORTK 출력설정
 7) PORTK HIGH로 초기화
 8) PORTC 출력설정
 9) PORTC LOW로 초기화
 10) S0핀 LOW 설정
 11) 숫자 5 HEX 값 디지털 신호 출력

********************************************************************/

#include <avr/io.h>
#include "gpio.h"

#define MAX_FND_SEL_NUM	6
#define MAX_FND_DB_NUM	8

#define FND_SEL_PORT	K
#define FND_DB_PORT		C

#define FND_5		0x6D

int main(void)
{
	int i = 0;
	int nFndStatus = 0;
		
	for(i=0; i<MAX_FND_SEL_NUM; i++)
	{
		pinMode(FND_SEL_PORT, i, OUTPUT);
		digitalWrite(FND_SEL_PORT, i, HIGH);
	}
	for(i=0; i<MAX_FND_DB_NUM; i++)
	{
		pinMode(FND_DB_PORT, i, OUTPUT);
		digitalWrite(FND_DB_PORT, i, LOW);
	}

	digitalWrite(FND_SEL_PORT, 0, LOW);
    while(1)
	{
		nFndStatus	=	FND_5;
		
		for(i=0; i<MAX_FND_DB_NUM; i++)
		{
			if((nFndStatus & (1<<i)))
			{
				digitalWrite(FND_DB_PORT, i, HIGH);
			}
			else
			{
				digitalWrite(FND_DB_PORT, i, LOW);
			}
		}	
    }
}