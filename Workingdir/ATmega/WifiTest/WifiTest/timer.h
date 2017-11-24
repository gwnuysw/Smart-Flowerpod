//1)
#ifndef __TIMER_H__
#define __TIMER_H__

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void TIMER_100mSInit (void);
void setElapsedTime100mSUnit(uint32_t Counter);
uint8_t isElapsed();

#endif  __TIMER_H__