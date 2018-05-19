#include "LabGPIO_0.hpp"

LabGPIO_0::LabGPIO_0(uint8_t port, uint8_t pin)
{
  port_num = port;
  pin_num = pin;
  switch (port)
  {
  case 0:
    LPC_GPIO = LPC_GPIO0;
    break;
  case 1:
    LPC_GPIO = LPC_GPIO1;
    break;
  case 2:
    LPC_GPIO = LPC_GPIO2;
    break;
  default:
    break;
  }
}

void LabGPIO_0::setAsInput()
{
  LPC_GPIO->FIODIR &= ~(1 << pin_num);
}

void LabGPIO_0::setAsOutput()
{
  LPC_GPIO->FIODIR |= (1 << pin_num);
}

void LabGPIO_0::setDirection(bool output)
{
  (output) ? setAsOutput() : setAsInput();
}

void LabGPIO_0::setHigh()
{
  LPC_GPIO->FIOSET = (1 << pin_num);
}

void LabGPIO_0::setLow()
{
  LPC_GPIO->FIOCLR = (1 << pin_num);
}

void LabGPIO_0::set(bool high)
{
  (high) ? setHigh() : setLow();
}

bool LabGPIO_0::getLevel()
{
  return LPC_GPIO->FIOPIN & (1 << pin_num);
}

LabGPIO_0::~LabGPIO_0()
{
  setAsInput();
}
