#ifndef __LAN_H__
#define __LAN_H__

#define MAX_RW_DATA_SIZE		256
#define IP_BYTE					4

// u8SocketStatus
#define  SOCKET_ST_NOT_CREATED          0 // no object
#define  SOCKET_ST_DISCONNECTED         1
#define  SOCKET_ST_CONNECTING           2
#define  SOCKET_ST_CONNECTED            4

// bNetStatus
#define DEV_NET_ST_CONNECTING      0
#define DEV_NET_ST_CONNECTED       1
#define DEV_NET_ST_DISCONNECTED    2

#define  STR_END_OF_CMD			0x0D

#define MAX_LAN_RX_DATA			256

typedef struct CLIENTInfo_tag
{
	uint8_t       u8SocketStatus;
	uint32_t      u32UnreadRxData;
	uint8_t		  u8RemoteIP[IP_BYTE];  // remote IP
	uint16_t	  u8RemotePort;			// remote port
}StClientInfo,*pStClientInfo;

#define  MAX_OBJ_NUM			8
typedef struct NETINFO_tag
{
	uint8_t  bDHCPMode;    // 0 => static IP ,  1 => DHCP IP
	uint8_t  bServerMode;  // 0 = Client Mode , 1 => Client Mode 
	uint8_t  bNetStatus;
	uint8_t  u8LocalIP[IP_BYTE];
	uint8_t  u8LocalSubnetMask[IP_BYTE];
	uint8_t  u8LocalGateway[IP_BYTE];
	StClientInfo  stCientObj[MAX_OBJ_NUM];
	void (*eventCalback)(int8_t type, int8_t firstArg, int8_t SecondArg);
	
}StNetInfo,*pStNetInfo;


void initLan();
void lanprint(const char *fmt,...);
void lanMain();
void LanSWReset();
uint8_t SetDHCPEnable();
uint8_t SetStaticIPEnable(uint8_t* IP,uint8_t* SubnetMask,uint8_t* Gateway);


uint8_t SetTCPServerMode(uint16_t u16ListenPort);
uint8_t SetTCPClientMode(uint8_t* ServerIP, uint16_t ServerPort);

//pStClientInfo GetEthClientInfo(int ClientIndex);

void SetEventCallback(void* func);
int  WriteEthData(int ClientIndex, uint8_t* pBuff, uint32_t dataSize);// max datasize is  MAX_RW_DATA_SIZE
int  ReadEthData(int ClientIndex, uint8_t* pBuff, uint32_t BuffSize); // max buffsize is  MAX_RW_DATA_SIZE

#define  WAIT_MAX_DELAY		10000 // unit mSec,  10Sec
//////////////////////////////////////////////////
//	Protocol
//////////////////////////////////////////////////
// EVENT
#define EVENT_HEADER_STR        "ET"
#define EVENT_SYSTEM            0
#define EVENT_SERVER           1
	#define EVENT_SERVER_LISTENING                   0
#define EVENT_CLIENT           2
	#define EVENT_CLIENT_CONNECTED                    0
	#define EVENT_CLIENT_DISCONNECTED                 1
	#define EVENT_CLIENT_SOCKET_CLOSED                2
	#define EVENT_CLIENT_BUFF_OVERFLOW                3
	#define EVENT_CLIENT_RECEIVED_DATA                4
#define EVENT_NET_STATUS        3
	#define EVENT_NET_STATUS_IP_ALLOCATED             0



#define CHAR_DELI	','
#endif //__LAN_H__

