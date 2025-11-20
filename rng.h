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

// ----------------------------------------------------- #includes for rng.c ---

#ifndef RNG_H
#define RNG_H

#include "stm32l4xx_hal.h"

// ---------- Function Prototypes ----------------------------------------------
void rng_init(void);
uint32_t rng(void);

#endif // RNG_H
