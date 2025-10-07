#include <stm32l432xx.h>
#include <stdio.h>

#define COUNTS_PER_REV   (408 * 4)      // quadrature x4 resolution
#define SAMPLE_PERIOD_MS 100

volatile int32_t last_count = 0;
volatile float encoder_speed_rps = 0.0f;

/* crude blocking delay assuming 4 MHz MSI clock */
static void delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * 4000; i++);
}

// For printf
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}

// Print velocity
void printVelocity(float velocity) {
  
  printf("Velocity: %f rev/s\n",  velocity);

}


void Encoder_Init(void)
{
    /* 1. Enable GPIOB and TIM2 clocks */
    RCC->AHB2ENR  |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    /* 2. Configure PB0/PB1 as AF1 (TIM2_CH3/TIM2_CH4) */
    GPIOB->MODER &= ~((3U << (0 * 2)) | (3U << (1 * 2)));   // clear mode bits
    GPIOB->MODER |=  ((2U << (0 * 2)) | (2U << (1 * 2)));   // AF mode
    GPIOB->AFR[0] &= ~((0xF << (0 * 4)) | (0xF << (1 * 4)));
    GPIOB->AFR[0] |=  ((1U << (0 * 4)) | (1U << (1 * 4)));  // AF1 = TIM2
    GPIOB->OSPEEDR |= (3U << (0 * 2)) | (3U << (1 * 2));    // high speed

    /* 3. Configure TIM2 in encoder mode using CH3 & CH4 */
    TIM2->CR1 = 0;                  // disable counter during setup
    TIM2->SMCR = 3;                 // SMS = 011 → encoder mode 3 (count on both inputs)
    TIM2->CCMR2 = (1U << 0) | (1U << 8);  // CC3S=01, CC4S=01 (map TI3 & TI4)
    TIM2->CCER = 0;                 // both channels non-inverted
    TIM2->ARR = 0xFFFFFFFF;         // 32-bit counter max
    TIM2->CNT = 0;                  // reset counter
    TIM2->CR1 |= TIM_CR1_CEN;       // enable counter
}

int32_t Encoder_GetCount(void)
{
    return (int32_t)TIM2->CNT;
}

float Encoder_GetSpeed(void)
{
    int32_t new_count = Encoder_GetCount();
    int32_t delta = new_count - last_count;
    last_count = new_count;

    encoder_speed_rps = ((float)delta / COUNTS_PER_REV) / (SAMPLE_PERIOD_MS / 1000.0f);
    return encoder_speed_rps;
}

int main(void)
{
    Encoder_Init();

    while (1) {
        delay_ms(SAMPLE_PERIOD_MS);
        float speed = Encoder_GetSpeed();  // revolutions per second
        printVelocity(speed);
    }
}
