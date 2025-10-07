// motor_polling.c
// Emily Kendrick
// ekendrick@hmc.edu
// 10/4/25
// Polls motor encoder inputs from a quadrature encoder and calculates average velocity over the last second.
// Prints velocity every second.

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
  v = (float) count / ((float) 4 * 408);
  return v;
}

// Print velocity
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

  int volatile count = 0;
  int volatile dir = 1;

  // Enable SYSCFG clock domain in RCC
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // Enable interrupts globally
  __enable_irq();


  // Turn on interrupt in NVIC_ISER
  NVIC->ISER[0] |= (1 << TIM2_IRQn);   

  // Enable interrupts for timer 2 (print timer)
  PRINT_TIM->DIER |= (1 << 0);
  // Disable interrupts for timer 15
  DELAY_TIM->DIER &= ~(1 << 0);
  
  int prevA;
  int currA = digitalRead(MOTORA_PIN);
  int prevB;
  int currB = digitalRead(MOTORB_PIN);
  
  while(1) {
    prevA = currA;
    currA = digitalRead(MOTORA_PIN);
    prevB = currB;
    currB = digitalRead(MOTORB_PIN);

    if (prevA != currA) {
      count += 1;
    }

    if (prevB != currB) {
      count += 1;
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
      flagPrint = 0; // Clear flag 
    }
    

  }
}

// Interrupt handler for TIM2
void TIM2_IRQHandler(void){
    
  // Clear interrupt
  NVIC->ICPR[0] |= (1 << TIM2_IRQn);

  // If so, reset timer 
  init_delay(PRINT_TIM, 1000);

  // Set flag
  flagPrint = 1;

}