#ifndef __BTHM11_H__
#define __BTHM11_H__

// MOD_RESET(PD6) (wifi module and  BT module reset )  => bt + wifi  reset 

   
// USART1 »ç¿ë , PD2 =>RX , PD3 =>TX
char gcBtMacNum[12];

void btInit();
void btprint(const char *fmt,...);
void btMain();
void btReadMac();
void wifiAndBTModuleReset();

#endif // __BTHM11_H__