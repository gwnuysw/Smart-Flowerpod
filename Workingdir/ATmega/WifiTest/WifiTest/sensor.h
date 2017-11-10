/********************************************************************

 파 일 : lcd.h

 소스 설명 :
 1) 입출력 관련 헤더파일 선언
 2) Delay 관련 헤더파일 선언
 3) 인터럽트 관련 헤더파일 선언                                                
 4) GPIO 관련 헤더파일을 선언
 5) TEMP 포트 및 핀 정의
 6) ACK/ NOACK 상수 값 정의
 7) Addr	Code(Command)	r/w
	000		0001  			1
	000		0010  			1
	000		0011  			1
	000		0011  			0
	000		1111  			0
 8) 온도와 습도를 나타내는 문자열 변수 선언
 9) 온/습도 센서와 I2C 관련 함수의 프로토타입을 선언
 
********************************************************************/

#ifndef	__LIB_SENSOR_H__
#define	__LIB_SENSOR_H__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "gpio.h"

#define TEMP_PORT		F
#define SCK		5
#define DATA	2

#define NOACK	0
#define ACK   	1

// SHT11 I2C Command
                            		// Addr	    Code(command)   r/w
#define MEASURE_TEMP		0x03	// 000   	0001  			1
#define MEASURE_HUMI		0x05	// 000   	0010  			1
//#define READ_STATUS_REG	0x07	// 000   	0011  			1
//#define WRITE_STATUS_REG	0x06	// 000   	0011  			0
//#define RESET				0x1e   	// 000   	1111  			0

enum { TEMP, HUMI };

void SHT11_Init (void);
void Connection_reset (void);
void Transmission_start (void);
unsigned short get_SHT11_data (unsigned char type);
unsigned char Write_byte (unsigned char value);
unsigned char Read_byte (unsigned char ack);
unsigned char Measure (unsigned short *p_value, 
						unsigned short *p_checksum,	unsigned char mode);
void calc_SHT11 (unsigned short p_humidity ,unsigned short p_temperature);

#endif	/* __LIB_SENSOR_H__ */
