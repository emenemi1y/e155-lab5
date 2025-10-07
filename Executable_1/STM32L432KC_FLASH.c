// STM32L432KC_FLASH.c
// Emily Kendrick, ekendrick@hmc.edu, 10/6/25
// Source code for FLASH functions

#include "STM32L432KC_FLASH.h"

void configureFlash(void) {
  FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
  FLASH->ACR |= FLASH_ACR_PRFTEN;
}