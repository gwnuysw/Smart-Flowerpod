#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Lan.h"

volatile StNetInfo  netInfo;
volatile pStNetInfo pNetInfo = (pStNetInfo)&netInfo;


volatile unsigned char g_LanRxData[MAX_LAN_RX_DATA]; // Rx buff
volatile unsigned char g_LanRxCurrentIndex = 0;      // Rx buffer index

void parsingData(uint8_t* data, uint32_t dataSize);
extern void debugprint(const char *fmt,...);
void putCh3(char ch)
{
	while (!(UCSR3A & 0x20));
	UDR3= ch;
}

ISR(USART3_RX_vect)
{
	g_LanRxData[g_LanRxCurrentIndex] = UDR3;
	g_LanRxCurrentIndex++;
	if(g_LanRxCurrentIndex >= MAX_LAN_RX_DATA)
		g_LanRxCurrentIndex = 0;
}

void lanprint(const char *fmt,...)
{
	va_list ap;
	char s[128];
	char *p_char = s;
	va_start(ap, fmt);
	vsprintf(s, fmt,ap);
	va_end(ap);
	while(0 != *p_char){
		putCh3(*p_char++);
	}
}

// 꼭 board port init 후 나중에 호출되어야 한다. 
// 
#define BAUD_RATE_LAN 115200L
void wireLanUartInit()
{
	// GPIO Init, USART3 TX (PJ1), RX(PJ0)
	DDRJ  &= 0xFC;										// "0" input, "1" output
	PORTJ |= 0x03;
	
	
	// Not Double mode, Not multi_communication
	UCSR3A = 0x00;								// x2 Mode
	UCSR3B = 0x98;
	UCSR3C = 0x06;
	
	unsigned int baud=1;   									// ??? baud???? "1"?? ????
	baud = ((F_CPU+(BAUD_RATE_LAN*8L))/(BAUD_RATE_LAN*16L)-1);      // baud?? ???ес?????? ????

	//	UBRR0H = 0x00;	// (FOSC / (16L * Baudrate)) - 1
	//	UBRR0L = 0x07;	// 0b00110011, (14745600 / (16 * 115200)) - 1 = 0x07
	//	UBRR0L = 0x5F;	// 0b00110011, (14745600 / (16 * 9600)) - 1 = 0x5F
	//	UBRR0L = 0x3F;	// 0b00110011, (14745600 / (16 * 14400)) - 1 = 0x40
	//	UBRR0L = 0x2F;	// 0b00110011, (14745600 / (16 * 19200)) - 1 = 0x2F
	//	UBRR0L = 0x17;	// 0b00110011, (14745600 / (16 * 38400)) - 1 = 0x17
	//	UBRR0L = 0x0F;	// 0b00110011, (14745600 / (16 * 57600)) - 1 = 0x0F

	UBRR3H=(unsigned char) (baud >>8);                            // baud ?????? ??????  H???????? ????
	UBRR3L=(unsigned char) (baud & 0xFF);                         // baud ?????? ??????  L???????  ????
	
	g_LanRxCurrentIndex = 0;
	
	
}

void initLan()
{
	int i;
	wireLanUartInit();
	
	pNetInfo->bNetStatus = DEV_NET_ST_DISCONNECTED;
	
	for(i = 0; i < MAX_OBJ_NUM ; i++ )
	{
		pNetInfo->stCientObj[i].u8SocketStatus = SOCKET_ST_NOT_CREATED;	
	}
//	for(i = 0; i < 50; i++)
	{
		lanprint("\r");
		lanprint("\r");
	}
	
}

extern void putCh2(char ch);

void lanMain()
{
#ifdef REDIRECT_PATH	
	int i;
	if ( !g_LanRxCurrentIndex )
		return;
	
	for(i = 0; i <g_LanRxCurrentIndex;i++)
	{
		putCh2(g_LanRxData[i]);
	}
	g_LanRxCurrentIndex = 0;
#else
	int i;
	int  endIndex = -1;
	if ( !g_LanRxCurrentIndex )
	{
		return;
	}
	
	uint8_t strTemp[256];
	for(i = 0; i <g_LanRxCurrentIndex;i++)
	{
		if ( g_LanRxData[i] == '\r' )
		{
			endIndex = i;
			memcpy(strTemp,(void*)g_LanRxData, endIndex);
			break; 
		}
	}
	if ( endIndex != -1)
	{
		g_LanRxCurrentIndex = 0;
		parsingData(strTemp , endIndex);
	}
	
#endif	
}



uint8_t  WaitNormalOK()
{
	while(1)  // expect  "+OK\n"  ==> 4 BYTE
	{
		if ( g_LanRxCurrentIndex > 0)
		{
			if (g_LanRxData[g_LanRxCurrentIndex-1] == STR_END_OF_CMD)
//			if (g_LanRxCurrentIndex >= 4)
			{
#if 0// test
{
	int j;
	for(j=0; j < g_LanRxCurrentIndex ;j++)
	{
		debugprint("[%02X]",g_LanRxData[j]);
	}
	debugprint("\r\n");
}
#endif				
				break;
			}				
		}
	}
	
	if (memcmp("+OK\r",(void*)g_LanRxData,g_LanRxCurrentIndex))
	{
		g_LanRxCurrentIndex = 0;		
		return 0; 	
	}
	g_LanRxCurrentIndex = 0;
	return 1;
}

int ParsingArgAndReturnNum(uint8_t* pData, int size, int* value )
{
	int i;
	int firstDeliIndex = -1;
	int SecondDeliIndex = -1;
	static int8_t strTemp[30];
	int  index = 0;
	
	for(i  = 0; i < size ; i++ )
	{
		if( (pData[i] == CHAR_DELI) || (pData[i] == STR_END_OF_CMD) )
		{
			if ( firstDeliIndex == -1 )
			{
				firstDeliIndex = i;
			}
			else
			{
				SecondDeliIndex = i;
				break;
			}
		}
		else if ( firstDeliIndex != -1)
		{
			strTemp[index] = pData[i];
			index++;
			if ( index >= 30)
			{
				debugprint("ParsingArgAndReturnNum error\r\n");
				return -1;
			}
		}
	}
	
	if ( ( SecondDeliIndex == -1 ) ||  ( firstDeliIndex == -1 ) )
	{
		return -1;
	}
	strTemp[index] = 0;
	
	
	//char  *stopstring;
	#if 0 // test
	debugprint( "ParsingArgAndReturnNum : %s\r\n" ,strTemp);
	#endif


	*value = atoi((char*)strTemp); // error  return 0
	//debugprint( "ParsingArgAndReturnNum : %d\r\n" ,*value);
	
	return SecondDeliIndex;
	
}

//  str  INPUT은  size 만큼 잡아 주어야 한다.
int ParsingArgAndReturnStr(int8_t* pData, int size , int8_t* str , int strSize)
{
	int i;
	int firstDeliIndex = -1;
	int SecondDeliIndex = -1;
	int  index = 0;
	
	for(i  = 0; i < size ; i++ )
	{
		if( (pData[i] == CHAR_DELI) || (pData[i] == STR_END_OF_CMD) )
		{
			if ( firstDeliIndex == -1 )
			{
				firstDeliIndex = i;
			}
			else
			{
				SecondDeliIndex = i;
				break;
			}
		}
		else if ( firstDeliIndex != -1)
		{
			str[index] = pData[i];
			index++;
			if ( index >= strSize)
			{
				debugprint("ParsingArgAndReturnStr error\r\n");
				return -1;
			}
		}
	}
	
	if ( ( SecondDeliIndex == -1 ) ||  ( firstDeliIndex == -1 ) )
	{
		return -1;
	}
	str[index] = 0;
	
	return SecondDeliIndex;
}

// Ip  => input   4byte
int ParsingArgAndReturnIP(int8_t* pData, int size , int8_t* Ip)
{
	int8_t strTemp[30];
	int result = ParsingArgAndReturnStr(pData, size,strTemp,30 );
	
	if ( result < 0 )
	return -1;
	

	#if 1 //test
	//strTemp
	debugprint("%08X\r\n",strTemp);
	#endif
	//Ip[3] = (uint8_t)(ip >> 24);
	//Ip[2] = (uint8_t)(ip >> 16);
	//Ip[1] = (uint8_t)(ip >> 8);
	//Ip[0] = (uint8_t)(ip);
	
	return result;
}


uint8_t  chToDec(uint8_t fCh,uint8_t sCh)
{
	int temp = fCh-0x30;
	int temp1 =sCh-0x30;
	
	return (uint8_t)(temp*10+temp1);
}

uint8_t WaitPacket(uint8_t* data,uint32_t* rxSize)
{
	while(1)  // expect  "+ET,T,FF,SS\n"  ==> 4 BYTE
	{
		if ( g_LanRxCurrentIndex > 0)
		{
			if (g_LanRxData[g_LanRxCurrentIndex-1] == STR_END_OF_CMD)
			{
				memcpy(data,(void*)g_LanRxData,g_LanRxCurrentIndex );
				*rxSize = g_LanRxCurrentIndex;
				g_LanRxCurrentIndex = 0;
				return 1; 
			}
		}	
						
	}	
	g_LanRxCurrentIndex = 0;
	return 0; 
}

void WaitnByte(uint8_t* data, uint32_t rxSize)
{
	while(1)
	{
		if ( g_LanRxCurrentIndex >= rxSize )
		{
			memcpy(data,(void*)g_LanRxData,rxSize);
			g_LanRxCurrentIndex =0;
			return;
		}
	}
}

uint8_t  WaitEvent(uint8_t *type, uint8_t* firstArg, uint8_t* SecondArg)
{
	while(1)  // expect  "+ET,T,FF,SS\n"  ==> 4 BYTE
	{
		if ( g_LanRxCurrentIndex > 0)
		{
			if (g_LanRxData[g_LanRxCurrentIndex-1] == STR_END_OF_CMD)
			{
				
#if 0// test 
{
	int j;
	for(j=0; j < g_LanRxCurrentIndex ;j++)
	{
		debugprint("[%02X]",g_LanRxData[j]);
	}
	debugprint("\r\n");	
}
#endif
				
				break;
			}
		}

		
	}
	
	if ( g_LanRxCurrentIndex != 12 )
	{
		g_LanRxCurrentIndex = 0;
		return 0;		
	}
	
	if (memcmp("+ET,",(void*)g_LanRxData,4))
	{
		g_LanRxCurrentIndex = 0;
		return 0;
	}	
	*type= chToDec('0',g_LanRxData[4]);
	*firstArg= chToDec(g_LanRxData[6],g_LanRxData[7]);
	*SecondArg= chToDec(g_LanRxData[9],g_LanRxData[10]);
	
	g_LanRxCurrentIndex = 0;
			
	return 1; 
}
void LanSWReset()
{
	lanprint("+SWRT\r");
	if(!WaitNormalOK())
	{
	}	
}
uint8_t SetDHCPEnable()
{

	uint8_t type, firstArg, SecondArg;
	lanprint("+DSMT,1\r");
	if (!WaitNormalOK() )
	{
		return 0;				
	}
	// Save 
	lanprint("+SAVE\r");
	if(!WaitNormalOK())
	{
		return 0;
	}
	
	lanprint("+SWRT\r"); // 설정 후에 꼭 reset을 시켜주어야 함.
	if (!WaitNormalOK() )
	{
		return 0;
	}
	// expect "+ET,3,00,00\n" 	
	if (!WaitEvent(&type,&firstArg,&SecondArg))  // reset 후  IP가 재 할당되는 것을 기다린다. 
	{		
		return 0; 
	}
	if ( (type == EVENT_NET_STATUS) && (firstArg == EVENT_NET_STATUS_IP_ALLOCATED))
	{
		debugprint("network IP allocated\r\n");
		pNetInfo->bDHCPMode = 1;
		pNetInfo->bNetStatus = DEV_NET_ST_CONNECTED;
		return 1;
	}
	
	debugprint("network IP not allocated\r\n");	
	return 0; 
}

uint8_t SetStaticIPEnable(uint8_t* IP,uint8_t* SubnetMask,uint8_t* Gateway)
{
	uint8_t type, firstArg, SecondArg;
	// static IP 
	lanprint("+DSMT,0\r");
	if (!WaitNormalOK() )
	{
		return 0;
	}
	// ip set 
	lanprint("+SLIP,%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d\r",IP[0],IP[1],IP[2],IP[3],
		SubnetMask[0],SubnetMask[1],SubnetMask[2],SubnetMask[3],Gateway[0],Gateway[1],Gateway[2],Gateway[3]);
	if(!WaitNormalOK())
	{
		return 0;
	}
	// Save
	lanprint("+SAVE\r");
	if(!WaitNormalOK())
	{
		return 0;
	}
	
	lanprint("+SWRT\r"); // 설정 후에 꼭 reset을 시켜주어야 함.
	if (!WaitNormalOK() )
	{
		return 0;
	}
			
	// expect "+ET,3,00,00\n"
	if (!WaitEvent(&type,&firstArg,&SecondArg))  // reset 후  IP가 재 할당되는 것을 기다린다.
	{
		return 0;
	}
	if ( (type == EVENT_NET_STATUS) && (firstArg == EVENT_NET_STATUS_IP_ALLOCATED))
	{
		debugprint("network IP allocated\r\n");
		pNetInfo->bDHCPMode = 1;
		pNetInfo->bNetStatus = DEV_NET_ST_CONNECTED;
		return 1;
	}
			
	// Save
	lanprint("+SAVE\r");
	if(!WaitNormalOK())
	{
		return 0;
	}
	
	lanprint("+SWRT\r"); // 설정 후에 꼭 reset을 시켜주어야 함.
	if (!WaitNormalOK() )
	{
		return 0;
	}
	// expect "+ET,3,00,00\n"
	if (!WaitEvent(&type,&firstArg,&SecondArg))  // reset 후  IP가 재 할당되는 것을 기다린다.
	{
		return 0;
	}
	if ( (type == EVENT_NET_STATUS) && (firstArg == EVENT_NET_STATUS_IP_ALLOCATED))
	{
		debugprint("network IP allocated\r\n");
		pNetInfo->bDHCPMode = 1;
		pNetInfo->bNetStatus = DEV_NET_ST_CONNECTED;
		return 1;
	}
	
	debugprint("network IP not allocated\r\n");
	return 0;	
}

uint8_t SetTCPServerMode(uint16_t u16ListenPort)
{
	lanprint("+CSCT,1,%d\r",u16ListenPort);
	if(!WaitNormalOK())
	{
		return 0;
	}
	return 1;	
}

uint8_t SetTCPClientMode(uint8_t* ServerIP, uint16_t ServerPort)
{
	uint8_t type, firstArg, SecondArg;	
	if (pNetInfo->bNetStatus !=  DEV_NET_ST_CONNECTED)
	{
		return 0;
	}
	lanprint("+CSCT,0,%u,%d.%d.%d.%d\r",ServerPort,ServerIP[0],ServerIP[1],ServerIP[2],ServerIP[3]);
	if(!WaitNormalOK())
	{
		return 0;
	}
	// expect "+ET,2,00,00\n"
	if (!WaitEvent(&type,&firstArg,&SecondArg))  // reset 후 IP가 재 할당되는 것을 기다린다.
	{
		return 0;
	}
	if ( (type == EVENT_CLIENT) && (firstArg == EVENT_CLIENT_CONNECTED))
	{
		debugprint("Client connected success.\r\n");
		pNetInfo->stCientObj[0].u8SocketStatus = SOCKET_ST_CONNECTED;
		return 1;
	}	
	debugprint("SetTCPClientMode connect fail\r\n");
	return 0;	
}

int  WriteEthData(int ClientIndex, uint8_t* pBuff, uint32_t dataSize)// max datasize is  MAX_RW_DATA_SIZE
{
	uint8_t buff[MAX_LAN_RX_DATA];
	uint32_t rxLen;
	int value,result;
#if 0	
	if ( pNetInfo->stCientObj[ClientIndex].u8SocketStatus != SOCKET_ST_CONNECTED)
	{
		debugprint("Client index:%d is not connected\r\n", ClientIndex);
		return -1;
	}
#endif	
	lanprint("+STDT,%d,%d\r",ClientIndex,dataSize);


	WaitPacket(buff,&rxLen);
	
#if 0
    debugprint("W:");
	for(int i = 0; i < rxLen; i++)
	{
		debugprint("[%02X]",buff[i]);
	}
	debugprint("\r\n");
#endif

	if ( memcmp("+OK,",buff,4))
	{
		
		WaitPacket(buff,&rxLen);
		if (memcmp("+OK,",buff,4)) // rx recevie  event가 들어올수 있음. 
		{
			debugprint("WriteEthData - fail.\r\n");
			return 0;			
		}
	}

	result = ParsingArgAndReturnNum(buff,rxLen, &value);

	if ( result < 0 )
	{
		debugprint("WriteEthData- not find end of packet\r\n");
		return 0;
	}
//	debugprint("data size to write:%d\r\n",value);
	// write data 
	for(int i =0; i < value; i++)
	{
		putCh3(pBuff[i]);
	}
	putCh3('\r');

	if(!WaitNormalOK())
	{
		return 0;
	}
		
//	debugprint("WriteEthData max write num %d\r\n",value);
	return value;
}

int  ReadEthData(int ClientIndex, uint8_t* pBuff, uint32_t BuffSize) // max buffsize is  MAX_RW_DATA_SIZE
{
	uint8_t buff[MAX_LAN_RX_DATA];
	uint32_t rxLen;
	int value,result;
		
	lanprint("+RCDT,%d,%d\r",ClientIndex,BuffSize);

	WaitPacket(buff,&rxLen);
	#if 0
	for(int i = 0; i < rxLen; i++)
	{
		debugprint("[%02X]",buff[i]);
	}
	debugprint("\r\n");
	#endif
	if ( memcmp("+OK,",buff,4))
	{
		debugprint("ReadEthData - OK fail.\r\n");
		return 0;
	}

	result = ParsingArgAndReturnNum(buff,rxLen, &value);

	if ( result < 0 )
	{
		debugprint("ReadEthData- not find end of packet\r\n");
		return 0;
	}
	
	debugprint("data size to read:%d\r\n",value);	
	
	WaitnByte(pBuff, value+1);
	
	if (pBuff[value] != '\r' )
	{
		debugprint("wrong end of pack\r\n");
	}

	
	return value;
}

void SetEventCallback(void* func)
{
	pNetInfo->eventCalback = func;
}

void parsingData(uint8_t* data, uint32_t dataSize)
{
#if 0
	debugprint("M:");
	for(int i=0; i < dataSize; i++)
	{
		debugprint("[%02X]",data[i]);
	}
	debugprint("\r\n");
#endif	
	// format => "+ET,X,YY,ZZ\r"
	if ( dataSize < 11)
	{
		return ;
	}
	
	if ( memcmp("+ET,",data,4))
	{
		return ;
	}
	
	uint8_t type= chToDec('0',data[4]);
	uint8_t  firstArg= chToDec(data[6],data[7]);
	uint8_t SecondArg= chToDec(data[9],data[10]);	
	
	pNetInfo->eventCalback(type,firstArg,SecondArg);
}