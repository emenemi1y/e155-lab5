// motor_read.c
// Emily Kendrick
// ekendrick@hmc.edu
// 10/4/25

#include "main.h"

volatile int flagA;
volatile int flagB;
volatile int flagPrint;

// For printf
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}

// Calculate velocity -- there are 120 sensors per revolution and 
// an interrupt is generated 4 times each sensor 
float calcVelocity(int count) {
  float v;
  v = (float) count / ((float) 4 * 120);
  return v;
}

void printVelocity(float velocity, int dir) {
  
  printf("Velocity: %f rev/s\n", (float) dir * velocity);

}


int main(void) {
  flagA = 0;
  flagB = 0;
  flagPrint = 0;
  
  // Enable both motor inputs as inputs:
  gpioEnable(GPIO_PORT_B);
  pinMode(MOTORA_PIN, GPIO_INPUT);
  pinMode(MOTORB_PIN, GPIO_INPUT);

  // Initialize timer
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
  RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
  initTIM(DELAY_TIM);
  initTIM(PRINT_TIM);

  // Take initial readings and initialize variables
  int volatile A;
  A = digitalRead(MOTORA_PIN);
  int volatile B;
  B = digitalRead(MOTORB_PIN);
  printf("hi %f", (float) 0.2222);

  int volatile count = 0;
  int volatile dir = 1;

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
  NVIC->ISER[0] |= (1 << TIM2_IRQn);   

  // Enable interrupts for timer 2 (print timer)
  printf("hi2");
  // PRINT_TIM->DIER |= (1 << 6);
  PRINT_TIM->DIER |= (1 << 0);
  // Disable interrupts for timer 15
  DELAY_TIM->DIER &= ~(1 << 0);
  printf("hi3");
     
  
  while(1) {
    delay_millis(DELAY_TIM, 10);
    

    // On edge of A
    if (flagA){
      count += 1;
      if (A) {
        // falling edge
        if (B) { dir = 1; }
        else { dir = -1; }
        A = 0;
      }
      else {
        // rising edge
        if (B) { dir = -1; }
        else { dir = 1; }
        A = 1;
      }

      flagA = 0; // Clear flag
    }
    
    // On edge of B
    if (flagB) {
      count += 1;
      if (B) {
        // falling edge
        if (A) { dir = -1; }
        else { dir = 1; }
        B = 0;
      }
      else {
        // rising edge
        if (A) { dir = 1; }
        else { dir = -1; }
        B = 1;
      }

      flagB = 0; // Clear flag
    }
    
    // Calculate and print velocity
    if (flagPrint) {
      if (count == 0){
        printVelocity(0, 0);
      }
      else{
        float velocity = calcVelocity(count);
        printVelocity(velocity, dir);
      }
      // Reset count
      count = 0;
    }

    flagPrint = 0; // Clear flag 
  }

}

void EXTI0_IRQHandler(void){
  // Check that the motor was what triggered the interrupt
  if (EXTI->PR1 & (1 << 0)) {
    // If so, clear the interrupt
    EXTI->PR1 |= (1 << 0);

    flagA = 1;

  }
}

void EXTI1_IRQHandler(void){
  // Check that the motor was what triggered the interrupt
  if (EXTI->PR1 & (1 << 1)) {
    // If so, clear the interrupt
    EXTI->PR1 |= (1 << 1);

  // Set flag
    flagB = 1;
  }
  
}

void TIM2_IRQHandler(void){
  // Check that the timer was what triggered the interrupt 
  if (NVIC->ICPR[0] & (1 << TIM2_IRQn)) {
    // clear interrupt
    printf("print1");
    NVIC->ICPR[0] |= (1 << TIM2_IRQn);
    printf("print2");

    // If so, reset timer 
    init_delay(PRINT_TIM, 1000);
    printf("print3");

    // Set flag
    flagPrint = 1;
    printf("print4");

  }
}