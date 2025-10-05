// motor_read.c
// Emily Kendrick
// ekendrick@hmc.edu
// 10/4/25

#include "main.h"

int flagA, flagB;

int main(void) {
  // Enable both motor inputs as inputs:
  gpioEnable(GPIO_PORT_B);
  pinMode(MOTORA_PIN, GPIO_INPUT);
  pinMode(MOTORB_PIN, GPIO_INPUT);

  // Initialize timer
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
  initTIM(READ_TIM);

  // Enable SYSCFG clock domain in RCC
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  // Configure EXTICR for the input button interrupt
  SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI0, 0b001); // Select PB0 
  SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI1, 0B001); // Select PB1

  // Enable interrupts globally
  __enable_irq();

  // Configure interrupt for both rising and falling edges of GPIO pins for motor inputs 
  // Configure mask bit
  EXTI->IMR1 |= (1 << gpioPinOffset(MOTORA_PIN)); 
  EXTI->IMR1 |= (1 << gpioPinOffset(MOTORB_PIN));

  // Enable rising edge trigger
  EXTI->RTSR1 |= (1 << gpioPinOffset(MOTORA_PIN));
  EXTI->RTSR1 |= (1 << gpioPinOffset(MOTORB_PIN));

  // Enable falling edge trigger
  EXTI->RTSR1 |= (1 << gpioPinOffset(MOTORA_PIN));
  EXTI->RTSR1 |= (1 << gpioPinOffset(MOTORB_PIN));

  // Turn on EXTI interrupt in NVIC_ISER
  NVIC->ISER[0] |= (1 << EXTI0_IRQn);
  NVIC->ISER[0] |= (1 << EXTI1_IRQn);

  // Take initial readings and initialize variables
  int prevA = digitalRead(MOTORA_PIN);
  int prevB = digitalRead(MOTORB_PIN);
  int currA = prevA;
  int currB = prevB;

  int prevTimA, prevTimB;
  int TimA, TimB;
  int counterOverflow = 0;
  
  while(1) {
    delay_millis(TIM2, 200);
    


  }

}

void EXTI0_IRQHandler(void){
  // Check that the motor was what triggered the interrupt
  if (EXTI->PR1 & (1 << 0)) {
    // If so, clear the interrupt
    EXTI->PR1 |= (1 << 0);

    // Set a flag 
    flagA = 1;

  }
}

void EXTI1_IRQHandler(void){
  // Check that the motor was what triggered the interrupt
  if (EXTI_PR1 & (1 << 1)) {
    // If so, clear the interrupt
    EXTI->PR1 |= (1 << 1)
