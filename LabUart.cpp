#include "LabUart.hpp"
#include "lpc_isr.h"
#include <stdio.h>
#include "FreeRTOS.h"

void labUART::init(unsigned int baud_rate)
{
    LPC_SC->PCONP |= (1<<24); // Power on clock
    LPC_SC->PCLKSEL1 &= ~(3<<16);
    LPC_SC->PCLKSEL1 |= (1<<16); //Peripheral clock selection
    LPC_PINCON->PINSEL4 &= ~(0xF << 16); // Clearing bits
    LPC_PINCON->PINSEL4 |= (0xA << 16); // Initialize UART2

    LPC_UART2->LCR = (1<<7); // Enable DLAB
    uint16_t dLL = sys_get_cpu_clock() / (16 * baud_rate) + 0.5;
    LPC_UART2->DLM = 0;
    LPC_UART2->DLL = dLL;
    LPC_UART2->LCR = 3; //Size = 8
}

void labUART::transmit(char ch)
{
    while(!(LPC_UART2->LSR && (1<<5))); // Wait for previous transmission
    //printf("\n--Transmitting '%c'--", ch);
    LPC_UART2->THR = ch; // Load the data to be transmitted
}
