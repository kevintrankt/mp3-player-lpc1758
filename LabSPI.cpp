#include "LabSPI.hpp"

LPC_SSP_TypeDef *LabSPI::SSP[] = {LPC_SSP0, LPC_SSP1};

LabSPI::LabSPI()
{
}

bool LabSPI::init(Peripheral peripheral, uint8_t data_size_select, FrameModes format, uint8_t divide)
{
  /**
   * 1) Powers on SPPn peripheral
   * 2) Set peripheral clock
   * 3) Sets pins for specified peripheral to MOSI, MISO, and SCK
   *
   * @param peripheral which peripheral SSP0 or SSP1 you want to select.
   * @param data_size_select transfer size data width; To optimize the code, look for a pattern in the datasheet
   * @param format is the code format for which synchronous serial protocol you want to use.
   * @param divide is the how much to divide the clock for SSP; take care of error cases such as the value of 0, 1, and odd numbers
   *
   * @return true if initialization was successful
   */
  bool success = true;
  // Check for valid variables
  if (peripheral != 0 && peripheral != 1)
  {
    success = false;
  }

  if (divide == 0 || divide % 2 == 1 || divide < 0 || divide > 8)
  {
    success = false;
  }

  sspx = peripheral;

  if (success)
  {
    if (peripheral == 1)
    {
      // Power: In the PCONP register (Table 46),
      LPC_SC->PCONP |= (0x1 << 10);
      // Clock: In PCLKSEL0 select PCLK_SSP1;
      LPC_SC->PCLKSEL0 &= ~(0x3 << 20);
      LPC_SC->PCLKSEL0 |= (1 << 20);
      //P0.7:9 init
      LPC_PINCON->PINSEL0 &= ~((3 << 18) | (3 << 16) | (3 << 14));
      LPC_PINCON->PINSEL0 |= ((2 << 18) | (2 << 16) | (2 << 14));

      //data size
      LPC_SSP1->CR0 = data_size_select - 1;

      //frame
      LPC_SSP1->CR0 |= (format << 4);

      //For AT45 flash SI pin is always latched on the rising edge of SCK, while output data
      //on the SO pin is always clocked out on the falling edge of SCK.
      //MS=0 (Master), SSE =1
      LPC_SSP1->CR1 = 0x2;
      LPC_SSP1->CPSR = 8 / divide; //SCK Frequency for Continuous Array Read(Low Frequency) is 33Mhx max. here we are setting it below it.
    }
    else
    {
      // // Power: In the PCONP register (Table 46),
      // LPC_SC->PCONP |= (0x1<<21);
      // LPC_SC -> PCONP |= (1<<10);
      //
      // LPC_SC->PCLKSEL1 &=~(0x3<<10); // reset SSP0
      // LPC_SC->PCLKSEL1 |= (1<<11); // set PCLK to full CPU speed (96MHz)
      //
      // LPC_PINCON->PINSEL1 &= ~((2<<2) | (2<<4));
      // LPC_PINCON->PINSEL1 |= ((1<<3)|(1<<5));
      //
      // //P0.16:18 init
      // //LPC_PINCON->PINSEL0 &= ~((3 << 0) | (3 << 2) | (3 << 4));
      // //LPC_PINCON->PINSEL0 |= ((2 << 0) | (2 << 2) | (2 << 4));
      // //P0.7:9 init
      // LPC_PINCON->PINSEL1 &= ~((3 << 0) | (3 << 2) | (3 << 4));
      // LPC_PINCON->PINSEL1 |= ((2 << 0) | (2 << 2) | (2 << 4));
      //
      // //data size
      // LPC_SSP0->CR0 = data_size_select-1;
      // //frame
      // LPC_SSP0->CR0 |= (format << 4);
      //
      // //MS=0 (Master), SSE =1
      // LPC_SSP0->CR1 = 0x2;
      // LPC_SSP0->CPSR = 8/divide; //SCK Frequency for Continuous Array Read(Low Frequency) is 33Mhx max. here we are setting it below it.
      //
      //
      // LPC_SSP0->CR0 = 7;          // 8-bit mode
      // LPC_SSP0->CR1 = (1 << 1);   // Enable SSP as Master
      // LPC_SSP0->CPSR = 8;

      LPC_SC->PCONP |= (1 << 10);
      LPC_SC->PCLKSEL1 &= ~(3 << 10);
      LPC_SC->PCLKSEL1 |= (1 << 11); // CLK / 2

      LPC_PINCON->PINSEL1 &= ~((2 << 2) | (2 << 4));
      LPC_PINCON->PINSEL1 |= ((1 << 3) | (1 << 5));

      LPC_SSP0->CR0 = 7;        // 8-bit mode
      LPC_SSP0->CR1 = (1 << 1); // Enable SSP as Master
      LPC_SSP0->CPSR = 8;
    }
  }
  return success;
}

uint8_t LabSPI::transfer(uint8_t send)
{
  if (sspx == SSP1)
  {
    LPC_SSP1->DR = send;
    while (LPC_SSP1->SR & (1 << 4))
      ;
    return LPC_SSP1->DR;
  }
  else
  {
    LPC_SSP0->DR = send;
    while (LPC_SSP0->SR & (1 << 4))
      ;
    return LPC_SSP0->DR;
  }
}
