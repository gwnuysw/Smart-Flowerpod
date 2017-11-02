/********************************************************************

 파 일 : adc.c
 
 소스 설명 :
 12) ADC 초기 설정 함수
 13) Timer0 Overflow0 ISP
  - clear Interrupt(SREG의 I비트 "0" 설정)
  - A/D 변환 값(Low 8 bit) 초기화
  - A/D 변환 값(High 4 bit) 초기화
  - 타이머 카운터(0.003초 설정)
  - 오버플로우 카운터 증가
  - 0.003s * 100 = 0.3s
  - 오버플로우 카운터 초기화
  - ADC 변환이 끝날떄 까지 대기
  - A/D 변환 값(Low 8 bit) 저장
  - A/D 변환 값(High 4 bit) 저장
  - A/D 변환 값(Total 12 bit)
  - set Interrupt(SREG의 I비트 "1" 설정)

 15) ADC 초기 설정 함수
  -Internal 2.56V Voltage Reference with external capacitor at AREF pin
   (ADMUX의 REFS(1:0) 비트를 "11"로 설정하여, 기준 전압을 내부 2.56V를 사용하도록 설정)
   (ADMUX의 ADLAR 비트를 "0"으로 설정하여, 우정렬 하도록 설정)
  - ADC Channel
   (ADMUX의 MUX(4:0) 비트를 "unAdcChannel"로 설정하여, 해당 ADC 채널을 사용)
  - ADC Module Enable, Free Running Mode, ADC Prescaler : 128
   (ADCSRA의 ADEN 비트를 "1"로 설정하여, A/D컨버터를 허용)
   (ADCSRA의 ADATE 비트를 "1"로 설정하여, ADC Auto Trigger Enalbe)
   (ADCSRA의 ADPS(2:0) 비트를 "111"으로 설정하여, 프리스케일러를 128분주로 설정)
   (By default, the successive approximation circuitry requires 
	an input clock frequency between 50kHz and 200kHz.)
  - ADC start conversion
   (ADCSR의 ADSC 비트를 설정하여 A/D 변환을 시작)
 
********************************************************************/

#include "adc.h"

// ADC Channel Set Function
// ATmega2560 ADC Channel : 0(ADC0) ~ 7(ADC7), Single Ended Input
void AdcInit(unsigned char ucAdcChannel)
{
	// Internal 2.56V Voltage Reference with external capacitor at AREF pin
//	ADMUX	=	0xC0 | ucAdcChannel;     		// ADC Channel
	ADMUX	=	(1<<REFS1) | (1<<REFS0) | ucAdcChannel;     		// ADC Channel

	// ADC Module Enable, Free Running Mode, ADC Prescaler : 128
	// ADC Auto Trigger Enable
//	ADCSRA	=	0xA7;
	ADCSRA	=	(1<<ADEN)|(1<<ADATE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
//	ADCSRA	|=	0x40;			// ADC start conversion
	ADCSRA	|=	(1<<ADSC);		// ADC start conversion
}


// ADC Read Function
unsigned int AdcRead(void)
{
	unsigned char ucLowValue = 0;
	unsigned char ucHighValue = 0;
	unsigned int unAdcValue = 0;
	while ((ADCSRA&0x10)==0x00);	// ADC 변환이 끝날떄 까지 대기
	
	ucLowValue = ADCL;				//하위 8bit
	ucHighValue	= ADCH ;			//상위 2bit
	
	unAdcValue = (ucHighValue << 8) | ucLowValue;
//	debugprint("unAdcValue : %d\r\n", unAdcValue);
	
	return unAdcValue;
}