#include "PWMDriver.hpp"

void PWMDriver::pwmSelectAllPins()
{
	LPC_PINCON->PINSEL4 = (1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10);
}

void PWMDriver::pwmSelectPin(PWM_PIN pwm_pin_arg)
{

	switch (pwm_pin_arg)
	{
	case 0:
		LPC_PINCON->PINSEL4 = (1 << 0);
		break;
	case 1:
		LPC_PINCON->PINSEL4 = (1 << 2);
		break;
	case 2:
		LPC_PINCON->PINSEL4 = (1 << 4);
		break;
	case 3:
		LPC_PINCON->PINSEL4 = (1 << 6);
		break;
	case 4:
		LPC_PINCON->PINSEL4 = (1 << 8);
		break;
	case 5:
		LPC_PINCON->PINSEL4 = (1 << 10);
		break;
	default:
		printf("pwmSelectPin Error\n");
		break;
	}
}

void PWMDriver::pwmInitSingleEdgeMode(uint32_t frequency_Hz)
{
	/*Init frequency??*/

	// Set PCPWM1 bit
	LPC_SC->PCONP |= (1 << 6);

	// Select PCLK_PWM1
	LPC_SC->PCLKSEL0 &= ~(3 << 12); // reset
	LPC_SC->PCLKSEL0 |= (1 << 12);  // set PCLK to full CPU speed (96MHz)

	// Reset Counters
	LPC_PWM1->TCR = 2;
	//Enable Counters
	LPC_PWM1->TCR = (1 << 0) | (0 << 3);

	// No Prescalar
	LPC_PWM1->PR = 0x0;

	// Reset on PWMMR0, resets the TC when it matches MR0
	LPC_PWM1->MCR = (1 << 1);

	// Match Register: max cycle time (Ton+Toff = 100)
	LPC_PWM1->MR0 = pwmcl / frequency_Hz;

	// Load Enable Register: Enables use of new PWM values once match occurs
	LPC_PWM1->LER = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6);

	// Enable PWM Output
	LPC_PWM1->PCR |= (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13) | (1 << 14);

	// Single Edge
	LPC_PWM1->PCR |= (0 << 2) | (0 << 3) | (0 << 4) | (0 << 5) | (0 << 6);

	/*Initialize PWM Channels and set channels to 0% Duty Cycle*/
	LPC_PWM1->MR1 = 0;
	LPC_PWM1->MR2 = 0;
	LPC_PWM1->MR3 = 0;
	LPC_PWM1->MR4 = 0;
	LPC_PWM1->MR5 = 0;
	LPC_PWM1->MR6 = 0;
}

void PWMDriver::setDutyCycle(PWM_PIN pwm_pin_arg, float duty_cycle_percentage)
{
	switch (pwm_pin_arg)
	{
	case 0:
		LPC_PWM1->MR1 = duty_cycle_percentage;
		LPC_PWM1->LER = (1 << 0);
		break;
	case 1:
		LPC_PWM1->MR2 = duty_cycle_percentage;
		LPC_PWM1->LER = (1 << 1);
		break;
	case 2:
		LPC_PWM1->MR3 = duty_cycle_percentage;
		LPC_PWM1->LER = (1 << 2);
		break;
	case 3:
		LPC_PWM1->MR4 = duty_cycle_percentage;
		LPC_PWM1->LER = (1 << 3);
		break;
	case 4:
		LPC_PWM1->MR5 = duty_cycle_percentage;
		LPC_PWM1->LER = (1 << 4);
		break;
	case 5:
		LPC_PWM1->MR6 = duty_cycle_percentage;
		LPC_PWM1->LER = (1 << 5);
		break;

	default:
		printf("setDutyCycle Error\n");
	}
}
