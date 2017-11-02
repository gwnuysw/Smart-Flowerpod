/********************************************************************

 파 일 : main.c
		 
 동 작 : 
	다수의 FND에 다양한 숫자를 디스플레이

 포트연결 :
	IOT SENSOR 모듈의 포트 J14(PORT K)를 7-SEG 모듈의 포트 FND_SEL(S0~S5)에 연결
	IOT SENSOR 모듈의 포트 J12(PORT C)를 7-SEG 모듈의 포트 FND_DB에 연결
	IOT SENSOR 모듈의 +3.3V와 GND 핀을 7-SEG 모듈의 J3 전원 컨넥터에 연결
	(화살표로 표시된 핀이 전원이며, 다른 핀이 GND)

 소스 설명 :
 1) 입출력 관련 헤더파일을 선언
 2) Delay 관련 헤더파일을 선언
 3) GPIO 관련 헤더파일을 선언
 4) FND_SEL과 FND_DB 핀 개수 정의
 5) FND_SEL 및 FND_DB 포트 정의
 6) 0~9, A~F, 점, 그리고 밑줄을 표시하기 위한 DATA 배열 선언
 7) FND의 위치와 DATA를 매개변수로 FND에 숫자를 표시하기 위한 함수
 8) 표시하고자 하는 FND 위치에 해당하는 핀을 LOW 설정
 9) 표시하고자 하는 숫자의 HEX 값 출력
 10) 3ms 지연
 11) PORTK 출력설정
 12) PORTK HIGH로 초기화
 13) PORTC 출력설정
 14) PORTC LOW로 초기화
 15) FND 표시 함수(FND 위치, DATA)
 
********************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "gpio.h"

#define MAX_FND_SEL_NUM	6
#define MAX_FND_DB_NUM	8

#define FND_SEL_PORT	K
#define FND_DB_PORT		C

unsigned char FND_DATA_TBL[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,
								0x7D,0x27,0x7F,0x67,0x77,0x7C,
								0x39,0x5E,0x79,0x71,0x08,0x80};

// S0 -> 0, S1 -> 1, S2 -> 2, S3 -> 3
void printFND(unsigned char position, unsigned char data)
{
	int i = 0;
	int nFndStatus = 0;
	
	for(i=0; i<MAX_FND_SEL_NUM; i++)
	{
		digitalWrite(FND_SEL_PORT, i, HIGH);
	}
	digitalWrite(FND_SEL_PORT, (0+position), LOW);
	
	nFndStatus	=	FND_DATA_TBL[data];
	
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
	_delay_ms(3);
}

void main(void)
{
	int i = 0;
	
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

	while(1)
	{
		printFND(0, 5);
		printFND(1, 6);
		printFND(2, 3);
		printFND(3, 4);
	}
}