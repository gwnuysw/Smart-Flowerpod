/********************************************************************

 파 일 : main.c
 
 동 작 : 
	Sensor 모듈의 CDS 센서로부터 측정한 조도의 디지털 수치를 기준으로
	어두운 경우, 점등하는 LED의 개수를 늘리고,
	밝은 경우, 점등하는 LED의 개수를 줄임
	조도를 변환한 ADC 값을 40단위의 17단계로 나누어 LED를 ON/OFF
	
 포트연결 :
	AVR 모듈의 포트 J14(PORT K)를 LED 모듈의 포트 GREEN에 연결
	AVR 모듈의 포트 J12(PORT C)를 LED 모듈의 포트 RED에 연결
	AVR 모듈의 포트 J13(PORT F)를 Sensor 모듈의 포트 ILUM에 연결

 소스 설명 :
 1) 입출력 관련 헤더파일 선언
	Delay 관련 헤더파일 선언
	인터럽트 관련 헤더파일 선언
 2) GPIO 관련 헤더파일 포함
	ADC 관련 헤더파일 포함
 3) LED 핀 개수 정의
 4) LED 포트 정의
 5) ADC 포트 정의
 6) LED 관련 상수 정의
 7) ADC 및 타이머/카운터 관련 변수 선언
 8) 함수 프로토타입 선언
 9) Timer0 Overflow0 ISR
  - 타이머 카운터(0.003초 설정)
  - 오버플로우 카운터 증가
  - 0.003s * 100 = 0.3s
  - 오버플로우 카운터 초기화
 10) 타이머 초기 설정 함수
  - Timer/Count0 사용
  - 동작모드와 클럭소스 및 프리스케일러 결정
  - 오버플로우까지 카운트를 45로 설정, 0.003s에 오버플로우 발생
   (1 / (14.7456 Mhz / 1024))ⅹ45 = 0.003s
  - 오버플로우 인터럽트 설정
 11) LED 초기 설정 함수
  - RED 포트 출력 설정
  - RED 포트 초기화
  - GREEN 포트 출력 설정
  - GREEN 포트 초기화
 12) LED 모듈 제어 함수
  - color : RED(0), GREEN(1)
 13) LED 초기 설정
   - PORTB = GREEN, 
   - PORTE = RED
  ACD 초기 설정
   - PORTF = ADC1
  set Interrupt(SREG의 I비트 "1" 설정)
 14) 0.3s가 경과하여 Flag가 1이 되면,
   - ADC 값을 읽어 변수에 저장
 15) 어두운 정도에 따라 점등하는 LED 개수를 증가
 
********************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "debug.h"
#include "gpio.h"
#include "adc.h"

#define MAX_LED_NUM		8

/* LED PORT Define*/
#define GREEN_PORT	K
#define RED_PORT	C

/* PORTF is used by ADC1 */

#define RED		0
#define GREEN	1

unsigned int gun_AdcValue	=	0;
unsigned char guc_OverflowCnt	=	0;
int n_flag_getADC = 0;

void TIMER_Init();
void LED_Init ();
void ADC_Init ();
void controlLED (int color, unsigned char data);

ISR (TIMER0_OVF_vect)	// Timer0 Overflow0 ISR
{
	TCNT0	=	0xff - 45;		// 0.003s
	guc_OverflowCnt++;

	if (guc_OverflowCnt == 100)		// 0.003s * 100 = 0.3s
	{
		guc_OverflowCnt	=	0;
		n_flag_getADC = 1;
	}
}

void TIMER_Init (void)
{
	// Timer/Count0 사용
	// 동작모드와 클럭소스 및 프리스케일러 결정
	TCCR0B	=	0x05;
	// (1 / (14.7456 Mhz  / 1024 prescaler )) * 45 => 0.003s
	// 오버플로우까지 카운트를 45으로 설정, 0.003s에 오버플로우 발생
	TCNT0	=	0xff - 45;		// 0.003s
	TIMSK0	|=	1 << TOIE0;		// 오버플로우 인터럽트
	TIFR0	|=	1 << TOV0;	// set Overflow Interupt Flag
}

void LED_Init (void)
{
	int i = 0;
	
	for(i=0; i<MAX_LED_NUM; i++)
	{
		pinMode(RED_PORT, i, OUTPUT);
		digitalWrite(RED_PORT, i, LOW);
		pinMode(GREEN_PORT, i, OUTPUT);
		digitalWrite(GREEN_PORT, i, LOW);
	}
}

// color : RED(0), GREEN(1)
void controlLED (int color, unsigned char data)
{
	int i = 0;
	unsigned char nLedStatus = 0;
	nLedStatus	=	data;
	
	for(i=0; i<MAX_LED_NUM; i++)
	{
		if((nLedStatus & (1<<i)))
		{
			if (color == RED)
				digitalWrite(RED_PORT, i, HIGH);
			else if (color == GREEN)
				digitalWrite(GREEN_PORT, i, HIGH);
		}
		else
		{
			if (color == RED)
				digitalWrite(RED_PORT, i, LOW);
			else if (color == GREEN)
				digitalWrite(GREEN_PORT, i, LOW);
		}
	}
}		

void main (void)
{
//	debugInit();
	
	LED_Init ();		// PORTB = GREEN, PORTE = RED
	AdcInit(1);			// PORTF = ADC1
	TIMER_Init();
	
	sei();
	
	while (1)
	{
		if(n_flag_getADC == 1)
		{
			n_flag_getADC = 0;
			
			gun_AdcValue	=	AdcRead();
//			debugprint("gun_AdcValue : %d\r\n", gun_AdcValue);			
		}
		
		// 어두운 정도에 따라 점등하는 LED 개수를 증가
	  	if (gun_AdcValue < 640 && gun_AdcValue >= 600)
		{	controlLED (GREEN, 0x80);	controlLED (RED, 0x00);	}
  		else if (gun_AdcValue < 600 && gun_AdcValue >= 560)
		{	controlLED (GREEN, 0xC0);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 560 && gun_AdcValue >= 520)
		{	controlLED (GREEN, 0xE0);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 520 && gun_AdcValue >= 480)
		{	controlLED (GREEN, 0xF0);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 480 && gun_AdcValue >= 440)
		{	controlLED (GREEN, 0xF8);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 440 && gun_AdcValue >= 400)
		{	controlLED (GREEN, 0xFC);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 400 && gun_AdcValue >= 360)
		{	controlLED (GREEN, 0xFE);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 360 && gun_AdcValue >= 320)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0x00);	}
		else if (gun_AdcValue < 320 && gun_AdcValue >= 280)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0x80);	}
		else if (gun_AdcValue < 280 && gun_AdcValue >= 240)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xC0);	}
		else if (gun_AdcValue < 240 && gun_AdcValue >= 200)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xE0);	}
		else if (gun_AdcValue < 200 && gun_AdcValue >= 160)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xF0);	}
		else if (gun_AdcValue < 160 && gun_AdcValue >= 120)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xF8);	}
		else if (gun_AdcValue < 120 && gun_AdcValue >= 80)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xFC);	}
		else if (gun_AdcValue < 80 && gun_AdcValue >= 40)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xFE);	}
		else if (gun_AdcValue < 40 && gun_AdcValue >= 0)
		{	controlLED (GREEN, 0xFF);	controlLED (RED, 0xFF);	}
		else
		{	controlLED (GREEN, 0x00);	controlLED (RED, 0x00);	}
	}
}
