#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <avr/interrupt.h>

void AdcInit(unsigned char ucAdcChannel);
unsigned int AdcRead(void);

#endif /* ADC_H */