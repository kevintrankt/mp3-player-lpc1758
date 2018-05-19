#ifndef LABUART_H
#define LABUART_H
#include "LPC17xx.h"     // LPC_UART2_BASE
#include "sys_config.h"  // sys_get_cpu_clock()

class labUART
{
public:
    void init(unsigned int baud_rate);
    void transmit(char ch);
};

#endif
