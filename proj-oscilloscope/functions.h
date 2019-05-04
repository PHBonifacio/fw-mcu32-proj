#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <stdint.h>

#define FREQ_80MHZ  0
#define FREQ_16MHZ  1

#define BAUD_115200 0
#define BAUD_9600   1

void PLL_Init(void);
void UART0_Init(uint32_t baud);
uint32_t sinFunction (float x);
uint32_t squareFunction (float x);
uint32_t getFunctionPoint (uint32_t(*func)(float),float dx);

#endif
