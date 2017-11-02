#ifndef __ESP8266_H__
#define __ESP8266_H__

// MOD_RESET(PD6) (wifi module and  BT module reset )  => bt + wifi  reset


// WIFI_EN  ( PD7 )
// EVENT
#define EVENT_RX_DATA		1


void wifiInit();
void wifiprint(const char *fmt,...);
void wifiMain();
void wificlearUartBuff();

void wifiAutoConnectDisable();
void wifiEchoDisable();
int wifiDisplayAPlist();
uint8_t wifiConnectAP(char* ssid, char* pwd);
uint8_t wifiDisplayLocalIP();

uint8_t wifiEnableDHCP();
uint8_t wifiConnectTCPServer(char* remoteIP,uint32_t remotePort);
uint8_t wifiSetEventCallback(void* func);

int wifiSendData(uint8_t* buff, int dataSize);
uint8_t wifiModeSet();

#endif // __DEBUG_H__