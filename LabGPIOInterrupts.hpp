#ifndef LABGPIOINTERRUPTS_H
#define LABGPIOINTERRUPTS_H

#include <stdio.h>
#include <cassert>
#include "LPC17xx.h"
#include "FreeRTOS.h"
typedef enum
{
  rising,
  falling,
  both
} InterruptCondition_E;

typedef void (*look_up)(void);

class LabGPIOInterrupts
{

private:
  /**
   * Your job here is to construct a lookup table matrix that correlates a pin
   * and port to a registered ISR. You may want to make additional probably need
   * more than one. Be clever here. How can you do this such that you and the
   * cpu do the least amount of work.
   look up table -
   */
  static look_up look_up_table[2][32];
  static bool instanceFlag;         // singleton
  static LabGPIOInterrupts *single; // singleton
  LabGPIOInterrupts();

public:
  static LabGPIOInterrupts *getInstance(); // singleton
  /**
     * LabGPIOInterrupts should be a singleton, meaning, only one instance can exist at a time.
     * Look up how to implement this.
     */
  // LabGPIOInterrupts();
  /**
     * 1) Should setup register "externalIRQHandler" as the EINT3 ISR.
     * 2) Should configure NVIC to notice EINT3 IRQs.
     */
  void init();
  /**
     * This handler should place a function pointer within the lookup table for the externalIRQHandler to find.
     *
     * @param[in] port         specify the GPIO port
     * @param[in] pin          specify the GPIO pin to assign an ISR to
     * @param[in] pin_isr      function to run when the interrupt event occurs
     * @param[in] condition    condition for the interrupt to occur on. RISING, FALLING or BOTH edges.
     * @return should return true if valid ports, pins, isrs were supplied and pin isr insertion was sucessful
     */
  bool attachInterruptHandler(uint8_t port, uint32_t pin, void (*pin_isr)(void), InterruptCondition_E condition);
  /**
     * After the init function has run, this will be executed whenever a proper
     * EINT3 external GPIO interrupt occurs. This function figure out which pin
     * has been interrupted and run the ccorrespondingISR for it using the lookup table.
     *
     * VERY IMPORTANT! Be sure to clear the interrupt flag that caused this
     * interrupt, or this function will be called again and again and again, ad infinitum.
     *
     * Also, NOTE that your code needs to be able to handle two GPIO interrupts occurring
     * at the same time.
     */
  static void externalIRQHandler(void);
  ~LabGPIOInterrupts();
};

#endif
