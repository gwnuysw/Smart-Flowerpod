/*
 * WifiTest.c
 *
 * Created: 2016-04-01 ?ㅽ썑 5:43:33
 *  Author: dogu
 */ 

 // wifi Client mode Test 
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "debug.h"
#include "Esp8266.h"
#include "timer.h"
#include "CDSsensor.h"
#include "adc.h"
#include "sensor.h"

#define SERVER_IP_STR	"192.168.1.33"   //라즈베리파이 켜고 수정해야할 부분 
#define SERVER_PORT		50001
//#define RED		0					
//#define GREEN	1				


unsigned int gun_AdcValue = 0;//-------------v수정부분
volatile unsigned short temp;
volatile unsigned short humi;
extern int n_flag_getADC;//--------------^수정부분

static void eventCallback(int eventType,uint8_t* rxBuff, int rxSize)
{
	if (eventType == EVENT_RX_DATA) 
	{
		debugprint("\r\nRX:%s\r\n", rxBuff);
	}
}


int main(void)
{
	uint8_t strTemp[256];
	unsigned char tempc, humic;   //변환된 온도 습도 저장 하는 변수
	
	debugInit();
	wifiInit();
	
	sei();
		
	debugprint("wifi test start\r\n");
	_delay_ms(1000);
	
	wificlearUartBuff();
	
	// UART echo disable 
	wifiEchoDisable();
	wificlearUartBuff();
	
	// register callback function 
	wifiSetEventCallback(eventCallback);
	
	// Auto connection disable
	wifiAutoConnectDisable();
	wificlearUartBuff();
	
	wifiModeSet();
	
	wifiEnableDHCP();
	
	debugprint("\r\nAP list display:\r\n");
	wifiDisplayAPlist();
	
	// connect  AP 
	debugprint("\r\n");
	if ( !wifiConnectAP("CSNET-301", "gwnucomse"))
	{
		debugprint("AP connected.\r\n");
	}
	else
	{
		debugprint("AP connection fail.\r\n");
	}
	// display  local allocated IP 
	debugprint("\r\n Allocated local IP:\r\n");
	wifiDisplayLocalIP();
	debugprint("\r\n");

	// TCP remote server connection
	if( !wifiConnectTCPServer(SERVER_IP_STR,SERVER_PORT))
	{
		debugprint("Connected Server.\r\n");
	}
	else
	{
		debugprint("Server connection fail.\r\n");
	}				
	debugprint("\r\n");
	
	// Send Test message 
	strcpy(strTemp,"test msessage: hello\r\n");
	wifiSendData(strTemp, sizeof("test msessage: hello\r\n"));
	
	 //여기서부터 실제 데이터가 가공된다. 
	TIMER_100mSInit ();
	setElapsedTime100mSUnit(100);

	debugprint("start Loop\r\n");	
	
	int counter = 0;
/*----------------led부분제거*/
	LED_Init ();		// PORTB = GREEN, PORTE = RED------------------------v수정 부분 LED CDS 설정
	AdcInit(1);			// PORTF = ADC1
	TIMER_Init();
		
	sei();//-------------------------------------------^수정부분
	
	SHT11_Init();  //humi temp 설정
	
    while(1)
    {//TODO:: Please write your application code 
		
		temp = get_SHT11_data (TEMP);	
		
		humi = get_SHT11_data (HUMI); 	
		
		tempc = printValue (TEMP, temp);
		humic = printValue (HUMI, humi);
		
	/*--------led부분 제거*/
		if(n_flag_getADC == 1)			//---------------------------------v조도 센서, led동작수정부분
		{
			n_flag_getADC = 0;
			
			gun_AdcValue	=	AdcRead();		//조도값중요!!!!!!
			//			debugprint("gun_AdcValue : %d\r\n", gun_AdcValue);
		}
		
		
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
		{	controlLED (GREEN, 0x00);	controlLED (RED, 0x00);	}//-------------------------^수정부분
		
		//데이터 전송 부분
        
		wifiMain();
		
		if ( isElapsed())
		{
			sprintf(strTemp,"hello:%d CDS : %d temp : %d humi : %d\n",counter++, gun_AdcValue, tempc, humic);		//가장 핵심적인 부분 
			wifiSendData(strTemp, strlen(strTemp));			//가장 핵심적인 부분  
			debugprint("TX:%s\r\n",strTemp);				//가장 핵심적인 부분  
		}
		
    }
}
