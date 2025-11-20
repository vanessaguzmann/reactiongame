/**
  ******************************************************************************
  * @file           : led_timer.h
  * @brief          : Header for led_timer.c.
  *                   ...
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

// ----------------------------------------------- #includes for led_timer.c ---

#ifndef LED_TIMER_H
#define LED_TIMER_H

#include "stm32l4xx_hal.h"
#include <stdint.h>      // for uint32_t an more
#include <math.h>		 // for math functions

// ---------- Type Switch (re-comment in one-line change, repeat in main.c) ----
typedef uint8_t var_type;
//typedef int32_t var_type;
//typedef int64_t var_type;
//typedef float var_type;
//typedef double var_type;
/* USER CODE END PTD */

// ---------- Defines ----------------------------------------------------------
#define LED_MASK (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);

// ---------- Function Prototypes ----------------------------------------------
void led_init(void);       	// enables GPIOC, config PC8..PC11 outputs, LEDs off
void timing_execution(var_type (*test_function)(var_type));	// times func. execution
void flash_led(void);
void flash_sequence(void);

#endif // LED_TIMER_H
