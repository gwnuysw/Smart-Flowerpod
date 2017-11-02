/********************************************************************

 파 일 : main.c
 
 동 작 : 
	초음파 센서로부터 측정한 거리 값을 확인하여, 
	특정 거리안에 사물이 검출되면 LED를 점등

 포트연결 :
	IOT SENSOR 모듈의 핀 SCL1을 HC-SR04 Ultrasonic Sensor 모듈의 Echo 핀에 연결
	IOT SENSOR 모듈의 핀 SDA1을 HC-SR04 Ultrasonic Sensor 모듈의 Trig 핀에 연결
	IOT SENSOR 모듈의 핀 +5V를 HC-SR04 Ultrasonic Sensor 모듈의 Vcc 핀에 연결
	IOT SENSOR 모듈의 핀 GND를 HC-SR04 Ultrasonic Sensor 모듈의 GND 핀에 연결
	IOT SENSOR 모듈의 핀 AD0을 LED 모듈의 S 핀에 연결
	IOT SENSOR 모듈의 핀 +5V를 LED 모듈의 V핀에 연결
	IOT SENSOR 모듈의 핀 GND(0V)를 LED 모듈의 G핀에 연결

 소스 설명 :
 1) 입출력 관련 헤더파일 포함
	Delay 관련 헤더파일 포함
	인터럽트 관련 헤더파일 포함
 2) GPIO 관련 헤더파일 포함
	외부 인터럽트 관련 헤더파일 포함
 3) 초음파 센서 포트 정의
	Echo 핀 정의
	Trigger 핀 정의
 4) LED 포트 정의
	LED 핀 정의
 5) LED 제어를 위한 거리 기준
 6) 함수 프로토타입 정의
 7) 58us(1cm기준)마다 증가하는 카운터
	토글 신호 변수
 8) Timer0 Overflow0 ISR
	- 타이머 카운터(58us 설정)
	- 58usec 마다 카운터 증가
 9) Interrupt0 ISR
	- 측정된 이전 거리값 초기화
	- Timer/Counter0 Overflow Interrupt Enable
	- 외부인터럽트(INT0)가 falling edge에서 발생
	- All Timer Disable
	- 외부인터럽트(INT0)가 rising edge에서 발생
 9) 타이머 초기 설정 함수
	- Timer/Count0 사용
	- 클럭소스 및 프리스케일러 결정
	- 오버플로우까지 카운트를 106으로 설정
	 (실제 59.6us정도 생성)
	- Timer/Counter0 Overflow Interrupt Enable
	- TCNT0 -> 256이 되면 오버플로우 발생
	- set Overflow Interupt Flag
 10) 초음파 센서 초기화 함수
	- Trigger 핀 출력 설정
	- Echo 핀 입력 설정
	- Trigger 핀 초기화
	- Echo 핀 초기화
 
 10) LED 초기 설정 함수
  - RED 포트 출력 설정
  - RED 포트 초기화
  - GREEN 포트 출력 설정
  - GREEN 포트 초기화
 11) LED 모듈 제어 함수
  - color : RED(0), GREEN(1)
 12) LED 초기 설정
   - PORTB = GREEN, 
   - PORTE = RED
  ACD 초기 설정
   - PORTF = ADC1
  set Interrupt(SREG의 I비트 "1" 설정)
 14) 0.3s가 경과하여 Flag가 1이 되면,
   - ADC 값을 읽어 변수에 저장
   - 센싱되는 값에 따라 점등하는 LED 색상 및 개수 변경
 
********************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "debug.h"
#include "gpio.h"
#include "exit.h"

#define HCSR04_PORT		D
#define ECHO	0			// Echo_pin
#define TRIG	1			// Trig_pin

#define LED_PORT	F		//GPIO PORTC
#define LED_PIN		0		//LED Pin

#define DISTANCE_LEVEL		50

//#define CM 1      //Centimeter
//#define INC 0     //Inch

void TIMER_Init (void);
void HCSR04_Init(void);
long Distance(long time, int flag);
void TrigPin_init();

volatile unsigned int vun_pulse_count = 0;
volatile unsigned char vuc_togle = 0;

ISR(TIMER0_OVF_vect)
{
	TCNT0	=	0xff - 106;		// 58usec
	
	vun_pulse_count++;   // 58usec 마다 증가
}

ISR(INT0_vect)
{
	if(vuc_togle == 0)			// 카운터 시작하고 다음 외부인터럽트(INT0)는 falling Edge에서 수행
	{
		vun_pulse_count = 0;	// 측정된 이전 거리값 초기화

		TIMSK0 = 1<<TOIE0;		// Timer/Counter0 Overflow Interrupt Enable

		EICRA = 1<<CS01;		// 외부인터럽트(INT0)가 falling edge에서 발생
	  
		vuc_togle = 1;
	} 
	else						//카운터를 정지시키고 다음 외부 인터럽트(IN0)는 Rising Edge에서 수행
	{
		TIMSK0 = 0x00;			//All Timer Disable
	  
		EICRA = (1<<CS01)|(1<<CS00);   //외부인터럽트(INT0)가 rising edge에서 발생

		vuc_togle = 0;
	}
}

void TIMER_Init (void)
{
	// Timer/Count0 사용
	// 동작모드와 클럭소스 및 프리스케일러 결정
	TCCR0B	=	1<<CS01;	//분주비 8 => 14.7456MHz -> 1.8432MHz
//	TCCR0B	=	1<<CS00;	//분주비 0 => 14.7456MHz
	
	// 오버플로우까지 카운트를 106으로 설정(실제 59.6us정도 생성)
	// 0.000001s에 오버플로우 발생, ( 1/(14745600 Hz/8 prescaler))ⅹ106 = 0.0000575086805s = 약58us
	TCNT0	=	0xff - 106;		// 58us
	// 0.000001s에 오버플로우 발생, 1/(14745600Hz)ⅹ15 = 0.000001017252604s = 약1us
//	TCNT0	=	0xff - 15;		// 1us
	
	TIMSK0	|=	1 << TOIE0;		// Timer/Counter0 Overflow Interrupt Enable, TCNT0 -> 256이 되면 오버플로우 발생
	TIFR0	|=	1 << TOV0;	// set Overflow Interupt Flag
}

void HCSR04_Init(void)
{
	pinMode(HCSR04_PORT, TRIG, OUTPUT);
	pinMode(HCSR04_PORT, ECHO, INPUT);

	digitalWrite(HCSR04_PORT, TRIG, LOW);
	digitalWrite(HCSR04_PORT, ECHO, LOW);	
}	

void TrigPin_init()
{
	digitalWrite(HCSR04_PORT, TRIG, LOW);
	_delay_us(2);
// pull the Trig pin to high level for more than 10us impulse 
	digitalWrite(HCSR04_PORT, TRIG, HIGH);
	_delay_us(10);
	digitalWrite(HCSR04_PORT, TRIG, LOW);
}

int main(void)
{
	debugInit();
	TIMER_Init();
	ExitInit(INT0, RISING);
	HCSR04_Init();
	
	pinMode(LED_PORT,LED_PIN,OUTPUT);
//	DDRK = 0x00;
	
	sei();
	
    while(1)
    {
		TrigPin_init();
		
		if(vun_pulse_count < DISTANCE_LEVEL)
		{
			digitalWrite(LED_PORT,LED_PIN,HIGH);			//LED ON
		}
		else
		{
			digitalWrite(LED_PORT,LED_PIN,LOW);				//LED OFF
		}
		debugprint("%d cm\r\n",vun_pulse_count);

		_delay_ms(100);
    }
}