#ifndef __CDS_H_
#define __CDS_H_

unsigned int gun_AdcValue	=	0;
unsigned char guc_OverflowCnt	=	0;
int n_flag_getADC = 0;

void TIMER_Init();
void LED_Init ();
void ADC_Init ();
void controlLED (int color, unsigned char data);
void CDSmain(void);
#endif
