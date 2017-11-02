#ifndef EXIT_H
#define EXIT_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LOW			0
#define CHANGE		1
#define FALLING		2
#define RISING		3

void ExitInit (int ExitNum, int SenseControl);

#endif /* EXIT_H */