/*
 * WiredLanTest.c
 *
 * Created: 2016-08-11 오후 5:12:17
 *  Author: dogu
 */ 

#include <avr/io.h>

#include <avr/interrupt.h>
#include <util/delay.h>
#include "lan.h"
#include "debug.h"
#include "timer.h"

uint8_t bClientConnected = 0;
uint8_t rxflag = 0;
#define SERVER_PORT		50001


static void rxEventCalback(int8_t type, int8_t firstArg, int8_t SecondArg)
{
	debugprint("RX Event type:%d, firstArg:%d, SecondArg:%d\r\n", type, firstArg,SecondArg);
	switch(type)
	{
		case EVENT_SYSTEM:
		break;
		case EVENT_SERVER:
		break;
		case EVENT_CLIENT:
		{
			switch(firstArg)
			{
				case EVENT_CLIENT_CONNECTED:
				bClientConnected = 1;
				break;
				case EVENT_CLIENT_DISCONNECTED:
				bClientConnected = 0;
				break;
				case EVENT_CLIENT_SOCKET_CLOSED:
				bClientConnected = 0;
				break;
				case EVENT_CLIENT_BUFF_OVERFLOW:
				rxflag = 1;
				break;
				case EVENT_CLIENT_RECEIVED_DATA:
				rxflag = 1;
				break;
			}
			
		}
		break;
		case EVENT_NET_STATUS:
		break;
	}
	
}

int main(void)
{

	uint8_t  RemoteIP[4];
	uint16_t RemotePort;
	uint8_t strTemp[256];
	
	debugInit();
	
	initLan();
	
	sei();
	LanSWReset();
	_delay_ms(3000);
	
	debugprint("RealIot Test \r\n");

	SetEventCallback(rxEventCalback);
	
	SetDHCPEnable();
	
	// server ip set	
	RemoteIP[0] = 192;
	RemoteIP[1] = 168;
	RemoteIP[2] = 10;
	RemoteIP[3] = 227;
	
	RemotePort = SERVER_PORT;

	if (!SetTCPClientMode(RemoteIP, RemotePort))
	{
		bClientConnected = 0;
		debugprint("Client connect fail\r\n");
	}
	else
	{
		bClientConnected = 1;
		debugprint("Client connect success\r\n");
	}

	TIMER_100mSInit ();
	setElapsedTime100mSUnit(100);	// 1 Sec

	int counter = 0;
	while(1)
	{
		
		lanMain();
		checkRxData();
		
		if ( isElapsed())
		{
			sprintf(strTemp,"hello:%d\r\n",counter++);
			WriteEthData(0,strTemp,strlen(strTemp));
			debugprint("TX:%s\r\n",strTemp);
		}
	}
}
void checkRxData()
{
	uint8_t buff[MAX_LAN_RX_DATA +1];
	int  readDataLength;
	if (rxflag)
	{			
		readDataLength = ReadEthData(0,buff,MAX_LAN_RX_DATA);

		rxflag = 0;
		debugprint("\r\nRX:%s\r\n", buff);		
	}
}

