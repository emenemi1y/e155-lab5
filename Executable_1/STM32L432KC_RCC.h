// STM32F401RE_RCC.h
// Emily Kendrick, ekendrick@hmc.edu, 10/6/25
// Header for RCC functions

#ifndef STM32L4_RCC_H
#define STM32L4_RCC_H

#include <stdint.h>
#include <stm32l432xx.h>

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void configurePLL(void);
void configureClock(void);

#endif