
#include "cds.h"

// cdsChennel : 0~7
void CDS_Init (int cdsChannel)
{
	AdcInit(cdsChannel);			// PORTF = ADC1
}