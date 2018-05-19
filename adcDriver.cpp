#include "adcDriver.hpp"

ADCDriver::ADCDriver()
{
}

void ADCDriver::adcInitBurstMode()
{
  // Power up ADC
  LPC_SC->PCONP |= (1 << 12);
  // clock 25 MHz
  LPC_SC->PCLKSEL0 &= ~(1 << 24);
  LPC_SC->PCLKSEL0 &= ~(1 << 25);
  // 100 / 8 = 12.5 MHz
  // Set Up Burst Mode & Power On ADC
  LPC_ADC->ADCR = ((7 << 8) | (1 << 16) | (1 << 21));
}

void ADCDriver::adcSelectPin(ADC_PIN adc_pin_arg)
{
  switch (adc_pin_arg)
  {
  case ADC_PIN_0_25:
    LPC_PINCON->PINSEL1 &= ~(0x3 << 18);
    LPC_PINCON->PINSEL1 |= (0x1 << 18);
    LPC_ADC->ADCR |= (1 << 2); // sample AD0.2
    break;
  case ADC_PIN_0_26:
    LPC_PINCON->PINSEL1 &= ~(0x3 << 20);
    LPC_PINCON->PINSEL1 |= (0x1 << 20);
    LPC_ADC->ADCR |= (1 << 3); // sample AD0.3
    break;
  case ADC_PIN_1_30:
    LPC_PINCON->PINSEL3 |= (0x3 << 28);
    LPC_ADC->ADCR |= (1 << 4); // sample AD0.4
    break;
  case ADC_PIN_1_31:
    LPC_PINCON->PINSEL3 |= (0x3 << 30);
    LPC_ADC->ADCR |= (1 << 5); // sample AD0.5
    break;
  default:
    break;
  }
}

float ADCDriver::readADCVoltageByChannel(uint8_t adc_channel_arg)
{
  int result = 0;
  float volts = 0;
  switch (adc_channel_arg)
  {
  //Stored in bits 4 to 15
  case 2:
    result = (LPC_ADC->ADDR2 >> 4) & 0xFFF;
    volts = (result * 3.3) / 4096.0;
    break;
  case 3:
    result = (LPC_ADC->ADDR3 >> 4) & 0xFFF;
    volts = (result * 3.3) / 4096.0;
    break;
  case 4:
    result = (LPC_ADC->ADDR4 >> 4) & 0xFFF;
    volts = (result * 3.3) / 4096.0;
    break;
  case 5:
    result = (LPC_ADC->ADDR5 >> 4) & 0xFFF;
    volts = (result * 3.3) / 4096.0;
    break;
  default:
    volts = -1;
    break;
  }
  return volts;
}
