/*
 * WifiTest.c
 *
 * Created: 2016-04-01 오후 5:43:33
 *  Author: dogu
 */ 

 // wifi Client mode Test 
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "debug.h"
#include "Esp8266.h"
#include "timer.h"


#define SERVER_IP_STR	"192.168.1.42"
#define SERVER_PORT		50001

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
	 
	TIMER_100mSInit ();
	setElapsedTime100mSUnit(100);

	debugprint("start Loop\r\n");	
	int counter = 0;
    while(1)
    {
        //TODO:: Please write your application code 
		wifiMain();
		
		if ( isElapsed())
		{
			sprintf(strTemp,"hello:%d\r\n",counter++);
			wifiSendData(strTemp, strlen(strTemp));
			debugprint("TX:%s\r\n",strTemp);
		}
		
    }
}
