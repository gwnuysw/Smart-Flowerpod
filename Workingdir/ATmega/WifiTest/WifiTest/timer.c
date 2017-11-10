#include "timer.h"

volatile uint32_t timerCouter = 0; 
volatile uint32_t setTimerCheckCounter = 0;
volatile uint8_t  flagOverflowSetTimer = 0;
  
  
void TIMER_100mSInit (void)
{
	// Timer/Count0 사용
	// 동작모드와 클럭소스 및 프리스케일러 결정
	TCCR0B	=	0x05;  //  fclk_io / 1024   
	// 0.01s에 오버플로우 발생, 1/(14745600Hz/1024)ⅹ144 = 0.01s
	// 오버플로우까지 카운트를 144로 설정
	TCNT0	=	0xff - 144;
	TIMSK0	|=	1 << TOIE0;	// Overflow Interupt Enable
	TIFR0	|=	1 << TOV0;	// set Overflow Interupt Flag
}


void setElapsedTime100mSUnit(uint32_t Counter)
{
	setTimerCheckCounter = Counter;
	timerCouter = 0;
}

uint8_t isElapsed()
{
	if (flagOverflowSetTimer )
	{
		flagOverflowSetTimer = 0;
		return 1;
	}
	
	return 0;
}
