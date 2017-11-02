#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "Esp8266.h"
#include "debug.h"

#define MAX_WIFI_RX_DATA	512
#define LINE_FEED			0x0A
#define CARRIAGE_RETURN		0x0D

volatile unsigned char g_wifiRxData[MAX_WIFI_RX_DATA]; // Rx buff
volatile unsigned char g_wifiRxCurrentIndex = 0;       // Rx buffer index

volatile unsigned char g_wifiIPDRxData[MAX_WIFI_RX_DATA];
volatile int g_wifiIPDRxDataLen = 0;


volatile void (*callbackFnc)(int eventType,uint8_t* rxBuff, int rxSize);

volatile uint8_t g_underRxDataFlag = 0;
volatile int g_rxDataLen = 0;
volatile uint8_t g_rxDataBuff[MAX_WIFI_RX_DATA];

void putCh0(char ch)
{
	while (!(UCSR0A & 0x20));
	UDR0= ch;
}

ISR(USART0_RX_vect)
{
	g_wifiRxData[g_wifiRxCurrentIndex] = UDR0;
	g_wifiRxCurrentIndex++;
	if(g_wifiRxCurrentIndex >= MAX_WIFI_RX_DATA)
	g_wifiRxCurrentIndex = 0;
}

void wifiAndBTModuleReset()
{
	// MOD_RESET(PD6) (wifi module and  BT module reset )  => bt + wifi  reset
	DDRD  &= 0xBF; // input										// "0" input, "1" output
	PORTD &= 0xBF; // low
	
	_delay_ms(30);
	DDRD  |= 0x40; // output
	_delay_ms(50);
	DDRD  &= 0xBF; // input
	PORTD |= 0x40; // high
	
	_delay_ms(2000);
}

void wifiEnable(uint8_t bEnable)
{
	if ( bEnable)
	{
		PORTD |= 0x80;  // high output
	}
	else
	{
		PORTD &= 0x7F;  // low output
	}
	_delay_ms(200);
	
}

// wifi module use USART0
// RX0  => PE0
// TX0  => PE1

#define BAUD_RATE_WIFI 115200L
void wifiInit()
{
	// USART0 use
	// PE0 => RX, PE1 => TX
	DDRE  &= 0xFC;										// "0" input, "1" output
	PORTE |= 0x00;
	
	// Not Double mode, Not multi_communication
	UCSR0A = 0x00;								// x2 Mode
	UCSR0B = 0x98;
	UCSR0C = 0x06;
	//	UBRR0H = 0x00;
	//	UBRR0L = 16;								// 115200
	
	unsigned int baud=1;   									// ??? baud???? "1"?? ????
	baud = ((F_CPU+(BAUD_RATE_WIFI*8L))/(BAUD_RATE_WIFI*16L)-1);      // baud?? ???ес?????? ????
	
	UBRR0H=(unsigned char) (baud >>8);                            // baud ?????? ??????  H???????? ????
	UBRR0L=(unsigned char) (baud & 0xFF);                         // baud ?????? ??????  L???????  ????
	
	// wifi enable init
	// WIFI_EN  ( PD7 )
	DDRD  |= 0x80; // output										// "0" input, "1" output
	PORTD |= 0x80; // default high
	
	wifiAndBTModuleReset();
	_delay_ms(100);
	wifiEnable(1);
	
	callbackFnc = NULL;
}

void wifiprint(const char *fmt,...)
{
	va_list ap;
	char s[128];
	char *p_char = s;
	va_start(ap, fmt);
	vsprintf(s, fmt,ap);
	va_end(ap);
	while(0 != *p_char){
		putCh0(*p_char++);
	}
}

extern void putCh2(char ch);


// return para
// 0 => success
// 1 => under rxing
// 2 => parcing fail

// +IPD,<len>:<data>
uint8_t parsingIPDRxData(uint8_t* buff, int buffLen)
{
	int deliIndex = -1;
	int rxLen;
	char  strTemp[10];
	for( int i = 0; i < buffLen ; i++)
	{
		if (buff[i] == ':')
		{
			deliIndex = i;
			break;
		}
	}
	if ( deliIndex == -1)
	{
		return 1;
	}
	if (deliIndex > 11 )
	{
		
		return 2;
	}
	memcpy(strTemp,buff+5, deliIndex - 5 );
	strTemp[deliIndex - 5] = NULL;
	
	rxLen = atoi(strTemp);
	
	if ( rxLen == 0)
	{
		return 2;
	}
	
	if ( (buffLen -deliIndex-1) < rxLen )
	{
		if ( (buffLen-deliIndex+1) == rxLen ) // message에  \r\n 붙은 경우 
		{
			g_wifiIPDRxDataLen = rxLen;	
			memcpy(g_wifiIPDRxData,buff+deliIndex+1,rxLen-2 );
			g_wifiIPDRxData[rxLen-2] = 0x0D;
			g_wifiIPDRxData[rxLen-1] = 0x0A;
			return 0;
		}
		return 1;
	}
	g_wifiIPDRxDataLen = rxLen;
	memcpy(g_wifiIPDRxData,buff+deliIndex+1,rxLen );
	return 0;
}

#define  MSG_UNDEFINED	0
#define  MSG_SEND_OK	1
#define  MSG_OK			2
#define  MSG_RX			3
#define  MSG_RXING		4
#define  MSG_ERROR		5
#define  MSG_FAIL		6
#define  MSG_ALREAD		7
#define  MSG_SEND_READY	8
#define  MSG_BYSY		9
#define  MSG_UNDER_MSG  10

uint8_t parsingMsg(uint8_t* buff, int buffLen)
{
#if 0 // test
debugprint("#:");
for(int i = 0; i <buffLen;i++)
{
	//putCh2(g_wifiRxData[i]);
	debugprint("[%02X]",buff[i]);
}
debugprint("\r\n");
#endif	
	if (!strncmp((char*)buff,"OK",2))
	{
		return MSG_OK;
	}
	else if (buff[0] == '>')
	{
		return MSG_SEND_READY;
	}
	else if ((buffLen >= 7) && !strncmp(buff,"SEND OK",7)  )
	{
		return MSG_SEND_OK;
	}
	else if ((buffLen >=4) &&  !strncmp(buff,"+IPD,",4) )
	{
		uint8_t temp;
		temp = parsingIPDRxData(buff, buffLen);
		if ( temp == 0)
			return MSG_RX;
		else if (temp == 1)
			return MSG_RXING;
		
		return MSG_UNDEFINED;	// fail
	}
	else if ((buffLen >= 5) && !strncmp(buff,"ERROR",5)  )
	{
		return MSG_ERROR;
	}
	else if ((buffLen >= 4) && !strncmp(buff,"FAIL",5)  )
	{
		return MSG_FAIL;
	}
	else if ((buffLen >= 7) && !strncmp(buff,"ALREADY",7)  )
	{
		return MSG_ALREAD;
	}
	else if ((buffLen>= 7) && !strncmp(buff,"busy...",7) )
	{
		return MSG_BYSY;
	}
	else if (buffLen>= 7)
	{
		return MSG_UNDEFINED;
	}
	
	return MSG_UNDER_MSG;
}

uint8_t rxDataProcess(uint8_t* msgBuff, uint8_t msgBuffLen)
{
	uint8_t strTemp[MAX_WIFI_RX_DATA];
	int  carriageReturnIndex;
	int lineFeedIndex;
	
	uint8_t syncDetected;
	uint8_t temp;
	uint8_t msgBuffIndex = 0;
	if (g_wifiRxCurrentIndex == 0 )
		return 0;
	
	if ((g_rxDataLen + g_wifiRxCurrentIndex) >= MAX_WIFI_RX_DATA )
	{
		g_rxDataLen = 0;
		g_underRxDataFlag = 0;
		return msgBuffIndex;
	}
	memcpy(g_rxDataBuff,g_wifiRxData,g_wifiRxCurrentIndex);
	g_rxDataLen += (int)g_wifiRxCurrentIndex;
	g_wifiRxCurrentIndex = 0;
	
#if 0 // test
	debugprint("$:");
	for(int i = 0; i <g_rxDataLen;i++)
	{
		//putCh2(g_wifiRxData[i]);
		debugprint("[%02X]",g_rxDataBuff[i]);
	}
#endif

	while(1)
	{
		carriageReturnIndex = -1;
		lineFeedIndex = -1;
		
		for ( int i = 0; i < g_rxDataLen ; i++ )
		{
			// check carriage return and line feed
			if ( carriageReturnIndex !=  -1)
			{
				if ( (g_rxDataBuff[i] ==  LINE_FEED) && ((carriageReturnIndex+1) == i ) )
				{
					lineFeedIndex = i;
					if ( lineFeedIndex == 1)//
					{
						memcpy(strTemp, g_rxDataBuff+lineFeedIndex+1, g_rxDataLen - lineFeedIndex -1);
						memcpy(g_rxDataBuff , strTemp, g_rxDataLen - lineFeedIndex -1);
						g_rxDataLen -= (lineFeedIndex + 1);
						break;
					}
					temp = parsingMsg(g_rxDataBuff, carriageReturnIndex);
					//					debugprint("M1:%d\r\n",temp);
					if ( msgBuffIndex <  msgBuffLen )
					{
						msgBuff[msgBuffIndex] = temp;
						msgBuffIndex++;
						
					}
					if ( (lineFeedIndex+1) == g_rxDataLen ) //when  line feed is data end
					{
						g_underRxDataFlag = 0;
						return msgBuffIndex;
					}
					memcpy(strTemp, g_rxDataBuff+lineFeedIndex+1, g_rxDataLen - lineFeedIndex -1);
					memcpy(g_rxDataBuff , strTemp, g_rxDataLen - lineFeedIndex -1);
					g_rxDataLen -= (lineFeedIndex + 1);
					
					break;
				}
			}
			else
			{
				if ( g_rxDataBuff[i] ==  CARRIAGE_RETURN )
				{
					carriageReturnIndex = i;
					continue;
				}
			}
		}
		
		if ( lineFeedIndex == -1) // no find sync, check rxing data
		{
			g_underRxDataFlag = 1;
			
			temp = parsingMsg(g_rxDataBuff, g_rxDataLen);
			//			debugprint("M2:%d\r\n",temp);
			if ((temp ==MSG_UNDER_MSG) || (temp ==MSG_RXING) )
			{
			}
			else
			{
				g_rxDataLen = 0;
			}
			msgBuff[msgBuffIndex] = temp;
			msgBuffIndex++;
			return msgBuffIndex;
		}
	}
	
	return msgBuffIndex;
}

/*
ex) receive data
[0D][0A][2B][49][50][44][2C][37][34][3A][FD][FE][43][00][00][00][11][08][00][24][00][FF][93][37][00]
[78][56][34][00][43][6F][6E][74][72][6F][6C][20][46][61][6E][20][31][2C][32][00][00][00][00][00][57]
[04][00][00][00][00][00][00][00][00][00][00][00][00][01][02][04][00][00][00][00][00][00][00][00][03]
[00][00][00][00][1E][00][00][00][FF]
*/

void wifiMain()
{
#if 0
	int i;

	if ( !g_wifiRxCurrentIndex )
	return;
	
	for(i = 0; i <g_wifiRxCurrentIndex;i++)
	{
		//putCh2(g_wifiRxData[i]);
		debugprint("[%02X]",g_wifiRxData[i]);
	}
	g_wifiRxCurrentIndex = 0;
#else
	int i;
	int  endIndex = -1;
	
	if ( g_wifiIPDRxDataLen )// no ipd data
	{
		if (callbackFnc != NULL )
		{
			callbackFnc(EVENT_RX_DATA,g_wifiIPDRxData,g_wifiIPDRxDataLen);
		}
		g_wifiIPDRxDataLen = 0;
		
	}
	
	if ( !g_wifiRxCurrentIndex )
		return;
	
#if 0 // test
	for(i = 0; i <g_wifiRxCurrentIndex;i++)
	{
		//putCh2(g_wifiRxData[i]);
		debugprint("[%02X]",g_wifiRxData[i]);
	}
#endif

	uint8_t msgBuff[4];
	uint8_t msgRxNum;
	msgRxNum = rxDataProcess(msgBuff,4);
	
	
	if ( !g_wifiIPDRxDataLen )// no ipd data
		return;

	if (callbackFnc != NULL )
	{
		callbackFnc(EVENT_RX_DATA,g_wifiIPDRxData,g_wifiIPDRxDataLen);
	}
	g_wifiIPDRxDataLen = 0;

	#endif
}

int WaitOKWithPrint()
{
	uint8_t strTemp[512];
	int currentIndex = 0;
	int i,j;
	
	/*
	\r\n
	+CWLAP:(0,"Use_yours",-64,"00:08:9f:71:fa:be",5)\r\n
	+CWLAP:(4,"KT_WLAN_1BF4",-85,"00:1f:96:1b:2f:4d",5)\r\n
	+CWLAP:(3,"U+Net7673",-77,"1c:a5:32:05:76:71",9)\r\n
	+CWLAP:(2,"CNDI_TKIP",-60,"00:24:a5:af:35:50",11)\r\n
	+CWLAP:(2,"CNDI_AES",-61,"02:24:a5:af:35:50",11)\r\n
	+CWLAP:(1,"CNDI_WEP",-61,"0a:24:a5:af:35:50",11)\r\n
	+CWLAP:(0,"iptime",-78,"64:e5:99:09:f9:a4",11)\r\n
	\r\n
	OK\r\n
	*/
	
	while(1)
	{
		if (g_wifiRxCurrentIndex)
		{
			for (i = 0; i < g_wifiRxCurrentIndex ; i++)
			{
				//
				strTemp[currentIndex++] = g_wifiRxData[i];
				//if((g_wifiRxData[i] !=0x0D) && (g_wifiRxData[i] !=0x0A)   )
				putCh2(g_wifiRxData[i]);
				
				if (g_wifiRxData[i] == 0x0A)
				{
					//debugprint("\r\n");
					if ( currentIndex >= 4) // "OK\r\n"
					{
						// check end  "OK\r\n"
						if (!strncmp((char*)strTemp,"OK",2))
						{
							g_wifiRxCurrentIndex = 0;
							return 0;
						}
						else if ((currentIndex >=4) &&  !strncmp(strTemp,"+IPD,",4) )
						{
							g_wifiRxCurrentIndex = 0;
							//							debugprint("error\r\n");
							return -1;
						}
						else if ((currentIndex >= 7) && !strncmp(strTemp,"ERROR",5)  )
						{
							g_wifiRxCurrentIndex = 0;
							//							debugprint("error\r\n");
							return -1;
						}
						else if ((currentIndex >= 6) && !strncmp(strTemp,"FAIL",5)  )
						{
							g_wifiRxCurrentIndex = 0;
							//							debugprint("fail\r\n");
							return -1;
						}
						else if ((currentIndex >= 7) && !strncmp(strTemp,"ALREAY",5)  )
						{
							g_wifiRxCurrentIndex = 0;
							return 0;
						}
						else if ((currentIndex >= 9) && !strncmp(strTemp,"SEND OK",7)  )
						{
							g_wifiRxCurrentIndex = 0;
							return 0;
						}
						// display
						//						strTemp[currentIndex] = NULL;
						#if 0 // test
						debugprint("\r\n");
						for(j = 0; j <currentIndex;j++)
						{
							//putCh2(g_wifiRxData[i]);
							debugprint("[%02X]",strTemp[j]);
						}
						debugprint("\r\n");
						
						#endif
						//						debugprint(strTemp);
						
					}
					currentIndex = 0;
				}
			}
			g_wifiRxCurrentIndex = 0;
		}
	}
}


int WaitSendReadyWithoutPrint()
{
	int counter = 0;
	uint8_t msgBuff[4];
	uint8_t msgRxNum;
	int i;
	uint8_t okFlag =0;
	uint8_t readyFlag = 0;
	while(1)
	{
		msgRxNum = rxDataProcess(msgBuff,4);
		for(i = 0 ; i< msgRxNum ; i++)
		{
			if (msgBuff[i] == MSG_SEND_READY)
			{
				
				readyFlag = 1;
			}
			else if (msgBuff[i] == MSG_OK)
			{
				okFlag = 1;
			}
			else if (msgBuff[i] == MSG_ERROR)
			{
				return -1;
			}
		}
		if ( readyFlag && okFlag )
		{
			return 0;
		}
		
		counter++;
		if (counter> 100)
		{
			debugprint("WaitSendReadyWithoutPrint counter err\r\n");
			return -1;
		}
		
		_delay_ms(10);
	}
}

int WaitSendOKWithoutPrint()
{
	int counter = 0;
	uint8_t msgBuff[4];
	uint8_t msgRxNum;
	int i;
	while(1)
	{
		msgRxNum = rxDataProcess(msgBuff,4);
		for(i = 0 ; i< msgRxNum ; i++)
		{
			if (msgBuff[i] == MSG_SEND_OK)
			return 0;
			else if (msgBuff[i] == MSG_ERROR)
			return -1;
		}
		
		counter++;
		if (counter> 100)
		{
			debugprint("WaitOKWithoutPrint counter err\r\n");
			return -1;
		}
		_delay_ms(10);
		
	}
}

int WaitOk() //"\r\nOK\r\n"
{
	uint8_t strTemp[256];
	int currentIndex = 0;
	uint8_t firstCarrageReturnDetect = 0;
	int i;
	
	while(1)
	{
		if (g_wifiRxCurrentIndex)
		{
			for (i = 0; i < g_wifiRxCurrentIndex ; i++)
			{
				strTemp[currentIndex++] = g_wifiRxData[i];
				if (g_wifiRxData[i] == 0x0A)
				{
					if ( !firstCarrageReturnDetect )
					{
						firstCarrageReturnDetect = 1;
					}
					else
					{
						// check end  "OK\r\n"
						if (!strncmp(strTemp+2,"OK",2))
						{
							g_wifiRxCurrentIndex = 0;
							return 0;
						}
						else if (!strncmp(strTemp+2,"ERROR",5)  )
						{
							g_wifiRxCurrentIndex = 0;
							return -1;
						}
						return -1;
					}
				}
			}
		}
	}
	return -1;
}

int wifiDisplayAPlist()
{
	wifiprint("AT+CWLAP\r\n");
	_delay_ms(2000);
	return WaitOKWithPrint();
}

uint8_t wifiConnectAP(char* ssid, char* pwd)
{
	wifiprint("AT+CWJAP_DEF=\"%s\",\"%s\"\r\n",ssid,pwd);
	
	return WaitOKWithPrint();
}


uint8_t wifiModeSet()
{
	wifiprint("AT+CWMODE=3\r\n");
	return WaitOKWithPrint();

}

uint8_t wifiDisplayLocalIP()
{
	wifiprint("AT+CIPSTA_CUR?\r\n");
	return WaitOKWithPrint();
}

uint8_t wifiEnableDHCP()
{
	wifiprint("AT+CWDHCP_CUR=1,1\r\n");
	return WaitOKWithPrint();

}


uint8_t wifiConnectTCPServer(char* remoteIP,uint32_t remotePort)
{
	wifiprint("AT+CIPSTART=\"TCP\",\"%s\",%u\r\n",remoteIP, remotePort);
	
	return WaitOKWithPrint();
}


uint8_t wifiSetEventCallback(void* func)
{
	callbackFnc = func;
	return 0;
}

int wifiSendData(uint8_t* buff, int dataSize)
{
	//	if (g_underRxDataFlag != 0 )
	if (0 )
	{
		debugprint("wifiSendData err. rx processing ,g_wifiRxCurrentIndex:%d\r\n",g_wifiRxCurrentIndex);
		return -1;
	}
	wifiprint("AT+CIPSEND=%d\r\n",dataSize);
	if ( WaitSendReadyWithoutPrint() < 0 )
	{
		debugprint("wifiSendData wait err -0");
		#if 1 // test
		debugprint("&:");
		for(int i = 0; i <g_rxDataLen;i++)
		{
			//putCh2(g_wifiRxData[i]);
			debugprint("[%02X]",g_rxDataBuff[i]);
		}
		#endif
		return -1;
	}
	
	for(int i = 0; i < dataSize ; i++ )
	{
		
		putCh0(buff[i]);
	}
	
	return WaitSendOKWithoutPrint();
	
}

void wificlearUartBuff()
{
	g_wifiRxCurrentIndex = 0;
	g_rxDataLen = 0;
}

void wifiAutoConnectDisable()
{
	wifiprint("AT+CWAUTOCONN=0\r\n");

	_delay_ms(50);
	WaitOk();
}

void wifiEchoDisable()
{
	wifiprint("ATE0\r\n");
	_delay_ms(100);
	
	wificlearUartBuff();
	
}

