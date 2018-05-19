#ifndef LABSPI_H
#define LABSPI_H

#include "LPC17xx.h"

enum FrameModes
{
  /* Fill this out based on the datasheet. */
  SPI = 0,
  TI = 1,
  Microwire = 2
};

enum Peripheral
{
  SSP0 = 0,
  SSP1 = 1
};
static Peripheral sspx;
class LabSPI
{
private:
  // SSP register lookup table structure
  static LPC_SSP_TypeDef *SSP[];
  // static const LPC_SSP_TypeDef * SSP[] = {LPC_SSP0, LPC_SSP1};

public:
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
  bool init(Peripheral peripheral, uint8_t data_size_select, FrameModes format, uint8_t divide);

  /**
   * Transfers a byte via SSP to an external device using the SSP data register.
   * This region must be protected by a mutex static to this class.
   *
   * @return received byte from external device via SSP data register.
   */
  uint8_t transfer(uint8_t send);

  LabSPI();
  // ~LabSPI();
};

#endif
