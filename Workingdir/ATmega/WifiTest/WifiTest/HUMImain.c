/********************************************************************

 파 일 : main.c
 
 동 작 : 
	Sensor 모듈의 온/습도 센서 SHT11에서 센싱한 아날로그 정보를 디지털로
	변환한 수치를 획득하여 2-Wire 직렬 버스 인터페이스를 통하여 MCU로 전송,
	전송된 정보를 온/습도를 계산하여 TEXT LCD 를 통하여 디스플레이

 포트연결 :
	IOT SENSOR 모듈의 포트 J14(PORT K)를 Text LCD 모듈의 포트 LCD_CD에 연결
	IOT SENSOR 모듈의 포트 J12(PORT C)를 Text LCD 모듈의 포트 LCD_DB에 연결
	IOT SENSOR 모듈의 포트 J13(PORT F)를 Sensor 모듈의 포트 TEMP에 연결
	IOT SENSOR 모듈의 +5V와 GND 핀을 Sensor모듈의 J3 전원 컨넥터에 연결
	(화살표로 표시된 핀이 전원이며, 다른 핀이 GND)

 소스 설명 :
 1) 센서 제어 관련 헤더파일 포함
 2) LCD 구동 관련 헤더파일 포함
 3) GPIO 관련 헤더파일을 포함
 4) 온/습도 값 저장 변수 선언
 5) 온/습도 값을 LCD에 표시
 6) 온/습도 센서 초기 설정
 7) LCD 초기 설정
 8) LCD 문자열 표시
 9) Move into 2nd Line
 10) LCD 문자열 표시
 11) Clear Display
 12) Sensing Temp
 13) Sensing Humi
 14) Cursor At Home(84ms delay)
 15) 온도 값을 LCD에 표시
 16) Move into 2nd Line
 17) 습도 값을 LCD에 표시

********************************************************************/

#include "sensor.h"
#include "gpio.h"

volatile unsigned short temp;
volatile unsigned short humi;
void printValue (unsigned char sense, unsigned short senseVal)
{
	unsigned char tens, units, tenths;
	if (sense == TEMP)
 		printString (" Temp: ");
 	else if (sense == HUMI)
	 	printString (" Humi: "); 
 	tens		=	senseVal / 100;			// 십의 자리
 	units		=	senseVal % 100 / 10;	// 일의 자리
	tenths	=	senseVal % 100 % 10;		// 소수점 첫째자리	 
 	if (tens > 0)
 		write_Data (tens+'0');
 	else
 		printString (" "); 
 	write_Data ((units)+'0');
	printString (".");				// 소수점 표현
	write_Data ((tenths)+'0');
}

/*
void Humimain (void)
{
	SHT11_Init();
	LCD_Init(LCD_CD_PORT, RS_PIN, RW_PIN, E_PIN, LCD_DB_PORT);
	
	printString ("TEMP/HUM Sensor");
	write_Command (0xC0);		 Move into 2nd Line 
	_delay_us (220);
	printString ("Operating..");
	_delay_ms(500);	
	write_Command(0x01);		 Clear Display 
	_delay_ms(9);	
	while (1)
	{
		temp = get_SHT11_data (TEMP);	// Sensing Temp
		_delay_ms (100);
		humi = get_SHT11_data (HUMI); 	// Sensing Humi
		_delay_ms (100);
		write_Command (0x02);		 Cursor At Home, 84ms
		_delay_ms (9);			
		printValue (TEMP, temp);			
		write_Command (0xC0);		 Move into 2nd Line 
		_delay_us (220);			
		printValue (HUMI, humi);			
		_delay_ms (300);
	}
}*/
