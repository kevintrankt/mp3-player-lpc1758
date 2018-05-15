#include "LabGPIOInterrupts.hpp"


LabGPIOInterrupts* LabGPIOInterrupts::single = NULL;

look_up LabGPIOInterrupts::look_up_table[2][32] = {
  {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
  {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
};

LabGPIOInterrupts::LabGPIOInterrupts(){
}

LabGPIOInterrupts* LabGPIOInterrupts::getInstance(){
  if(single == NULL){
    single = new LabGPIOInterrupts();
  }
  return single;
}

void LabGPIOInterrupts::init(){
  isr_register(EINT3_IRQn, externalIRQHandler);
  NVIC_EnableIRQ(EINT3_IRQn);
}

bool LabGPIOInterrupts::attachInterruptHandler(uint8_t port, uint32_t pin, void (*pin_isr)(void), InterruptCondition_E condition){
  look_up_table[port][pin] = pin_isr;
  if (port == 2){
    if(pin > 9){
      return false;
    }
    switch(condition){
      case falling:
        LPC_GPIOINT->IO2IntEnF |=(1<<pin);
        break;
      case rising:
        LPC_GPIOINT->IO2IntEnR |=(1<<pin);
        break;
      case both:
        LPC_GPIOINT->IO2IntEnR |=(1<<pin);
        LPC_GPIOINT->IO2IntEnF |=(1<<pin);
        break;
      default:
        break;
    }
  }
  if (port == 0){
    if(pin > 1 && pin < 26){
      return false;
    }
    switch(condition){
      case falling:
        LPC_GPIOINT->IO0IntEnF |=(1<<pin);
        break;
      case rising:
        LPC_GPIOINT->IO0IntEnR |=(1<<pin);
        break;
      case both:
        LPC_GPIOINT->IO0IntEnR |=(1<<pin);
        LPC_GPIOINT->IO0IntEnF |=(1<<pin);
        break;
      default:
        break;
    }
  }
  return true;
}

void LabGPIOInterrupts::externalIRQHandler(void){
  for(int pin = 0; pin < 32; pin++){
    if(LPC_GPIOINT->IO0IntStatR & (1<<pin) || LPC_GPIOINT->IO0IntStatF & (1<<pin)){
      // printf("Port 0 interrupt on pin %i\n", pin);
      // func();
      look_up x = look_up_table[0][pin];
      x();
      LPC_GPIOINT->IO0IntClr=(1<<pin);
      break;
    }
  }
  for(int pin = 0; pin < 32; pin++){
    if(LPC_GPIOINT->IO2IntStatR & (1<<pin) || LPC_GPIOINT->IO2IntStatF & (1<<pin)){
      LPC_GPIOINT->IO2IntClr=(1<<pin);
      // func();
      look_up x = look_up_table[2][pin];
      x();
      // printf("Port 2 interrupt on pin %i\n", pin);
      break;
    }
  }
}


LabGPIOInterrupts::~LabGPIOInterrupts(){
  instanceFlag = false;
}
