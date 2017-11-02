/********************************************************************

 파일명 : main.c
 
 동 작 : 
	Buzzer 모듈에 특정 주파수의 신호를 출력하여 계이름에 해당하는 소리를 발생
	동작을 실행하면, 낮은 "도"부터 높은 "도"까지 
	한 옥타브(8개)에 해당하는 음계의 소리를 차례로 발생
	
 포트연결 :
	IOT SENSOR  모듈의 포트 J6(PORT E)의 핀 11(OC3A)을 
	Buzzer 모듈의 포트 BUZZER의 핀 Buzzer에 연결
	
	IOT SENSOR 모듈의 포트 J12(PORT C)의 핀 1(PC0)을 
	Buzzer 모듈의 포트 BUZZER의 핀 Buzzer에 연결
	IOT SENSOR 모듈의 +3.3V와 GND 핀을 Buzzer 모듈의 J3 전원 컨넥터에 연결
	(화살표로 표시된 핀이 전원이며, 다른 핀이 GND)

 소스 설명 :
  1) 입출력 관련 헤더파일 포함
	 Delay 관련 헤더파일 포함
	 인터럽트 관련 헤더파일 포함
	 GPIO 관련 헤더파일 포함
  2) Buzzer 포트 및 핀 정의
  3) Buzzer Play/Stop을 위한 상수 정의
  4) 펄스의 폭 변조를 위한 10us당 카운트 정의
	 10us 마다 카운트하므로 약 95번 카운트가 발생하면 신호를 HIGH<->LOW 변경
  5) 7음계에서 각 음에 해당하는 주파수 값 정의
  6) 주파수 생성을 위한 10us 카운터 변수
	 1초 시간 측정을 위한 카운터 변수
	 토글까지의 시간을 카운트하는 변수
	 주파수 신호 변수
	 연주 정지 확인 변수
  7) 관련 함수 프로토타입 정의
  8) Interupt Service Routine(Timer 0 Overflow)
	 - 오버플로우까지 113번 카운트하여 인터럽트 발생
	 - 생성하는 주파수를 위한 10us 카운트가 토글까지의 카운트와 같거나 클 때,
	   주파수 신호를 토글함
	 - (인터럽트 루틴과 메인 함수 사이를 이동하며 딜레이가 발생하므로 루틴 안에 위치)
  9) Timer/Count0 사용
	 - 동작모드와 클럭소스 및 프리스케일러 결정
	 - fclk_io / 1 (No prescaling)
	 - 오버플로우까지 카운트를 147로 설정하면,
	 - 계산 결과에 따라 10us마다 타이머 인터럽트가 발생되어야 하지만
	 - 명령어 처리 속도가 발생하여 실제 측정치는 12.8us 정도로 확인
	 - 따라서 타이머 카운트를 약 10us로 맞추기 위해 카운트를 113으로 설정
  10) Buzzer 핀을 출력 설정 
	  Buzzer 핀을 LOW(0) 상태로 초기화
  11) 각 음계에 해당하는 주파수 값을 선택하는 함수
  12) 주파수 발생 정지 함수
  13) Buzzer 초기화
	  타이머 초기화
	  Buzzer를 연주 가능한 Flag 상태로 변경
	  인터럽트 설정
  14) 0.5s가 경과하면,
	  Buzzer가 연주 가능한 상태이면,
	  음계에 해당하는 주파수를 통해 토글까지 시간을 계산하여 변수에 전달
	  루프를 돌면서 주파수를 변경
	  주파수를 8번째 변경 할 때,
	  다시 0번째부터 실행할 수 있도록 초기화
	  Buzzer는 한 옥타브가 끝나면 연주를 정지하도록 변수 입력
	  연주 정지 Flag를 확인 후, 주파수 발생 정지

********************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "gpio.h"

#include "debug.h"

#define	BUZZER_PORT		C
#define	BUZZER_PIN		0

#define PLAY	0
#define STOP	1

//#define DEBUG

// (1s/523)/2 = 0.000956s = 0.956ms => 523Hz는 956us 마다 HIGH/LOW를 반복하는 신호
// 10us 마다 카운트하므로 약 95번 카운트가 발생하면 신호를 HIGH<->LOW 변경
#define PWM(x)		(unsigned int)(((1000000*1/x)/2)/10)		// 10us unit

#define DO_L		523
#define RE			587
#define MI			659
#define FA			698
#define SOL			784
#define RA			880
#define SI			987
#define DO_H		1046

volatile uint32_t vunCouterForFreq_10us = 0; 
volatile uint32_t vunCouterForTime_1s = 0; 
volatile uint32_t vunCountUntilToggle = 0;
volatile uint8_t  vunFrequencySignal = 1;
int gnFlag_stopPlay = STOP;

void TIMER_Init (void);
void BUZZER_Init (void);
int selectSevenScale (int scale);
void stopFreq(void);

ISR(TIMER0_OVF_vect) {

	TCNT0	=	0xff - 113;
	
#ifdef DEBUG
//	스코프를 통한 디버깅 목적으로 신호 발생
	PORTK ^= 0x01;
#endif

	vunCouterForFreq_10us++;
	vunCouterForTime_1s++;

	// 인터럽트 루틴과 메인 함수 사이를 이동하며 딜레이가 발생하므로 루틴 안에 위치.
	if ( vunCouterForFreq_10us >= vunCountUntilToggle )
	{
		vunCouterForFreq_10us = 0;
	
		vunFrequencySignal	^=	1;
		digitalWrite(BUZZER_PORT, BUZZER_PIN, vunFrequencySignal);		
	}
}

void TIMER_Init (void)
{
	// Timer/Count0 사용
	// 동작모드와 클럭소스 및 프리스케일러 결정
	TCCR0B	=	1<<CS00;  //  fclk_io / 1 (No prescaling)   
	
	// 오버플로우까지 카운트를 147로 설정하는 경우,
	// 1/(14745600Hz)ⅹ147 = 0.00000996907552083s = 10us
	// 계산 결과에 따라 10us마다 타이머 인터럽트가 발생되어야 하지만
	// 명령어 처리 속도가 발생하여 실제 측정치는 12.8us 정도로 확인
	// 따라서 타이머 카운트를 약 10us로 맞추기 위해 카운트를 113으로 설정
//	TCNT0	=	0xff - 147;
	TCNT0	=	0xff - 113;
	
	TIMSK0	|=	1 << TOIE0;	// Overflow Interupt Enable
	TIFR0	|=	1 << TOV0;	// set Overflow Interupt Flag
}

void BUZZER_Init (void)
{
	pinMode(BUZZER_PORT, BUZZER_PIN, OUTPUT);
	digitalWrite(BUZZER_PORT, BUZZER_PIN, LOW);
}

int selectSevenScale (int scale)
{
	int _ret = 0;
	switch (scale)
	{
		case 0:
			_ret	=	DO_L;
			break;
		case 1:
			_ret	=	RE;
			break;
		case 2:
			_ret	=	MI;
			break;
		case 3:
			_ret	=	FA;
			break;
		case 4:
			_ret	=	SOL;
			break;
		case 5:
			_ret	=	RA;
			break;
		case 6:
			_ret	=	SI;
			break;
		case 7:
			_ret	=	DO_H;
			break;
	}
	return _ret;
}


void stopFreq(void)
{
	TCCR0B	=	0x00;	// No clock source (Timer/Counter stoppeed)
	digitalWrite(BUZZER_PORT, BUZZER_PIN, LOW);
}	

void main (void)
{
	int scale = 0;

#ifdef DEBUG
	debugInit();

	// PORTK 초기화
	DDRK |= 0x01;
	PORTK = 0x00;
#endif
	
	BUZZER_Init();
	TIMER_Init();		// 10us

	gnFlag_stopPlay = PLAY;
	
	sei();

	while(1)
	{
//		if ( vunCouterForTime_1s >= 100000)		// 1s, 시간이 정확하지 않음.
		if ( vunCouterForTime_1s >= 50000)		// 0.5s
		{
			vunCouterForTime_1s = 0;
			if (gnFlag_stopPlay == PLAY)
			{
				vunCountUntilToggle = PWM(selectSevenScale(scale));
	//			debugprint("PWM : %d\r\n",PWM(selectSevenScale(scale)));
				scale++;
				if (scale>=8)
				{
					scale = 0;
					gnFlag_stopPlay = STOP;
				}					
			}
			else
			{
				stopFreq();
			}									
		}
	}
}
