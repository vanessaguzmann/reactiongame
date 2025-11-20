/**
  ******************************************************************************
  * @file           : rng.c
  * @brief          : rng program body
  * @editors		: Vanessa G
  ******************************************************************************
  * * wiring    	: n/a
  * * attachment	: n/a
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
  * REVISION HISTORY
  * 11/19/25	Created file
  ******************************************************************************
*/

#include "rng.h"
#include "main.h"

/*
 * Function 1/2:  rng_init
 * --------------------
 * initializes random number generator, helper function
 *
 *	takes in: nothing
 *
 *  returns: random value from 0.. 4294967296
 */
void rng_init(void)
{
   RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;     // Enable RNG clock
   RCC->CRRCR |= RCC_CRRCR_HSI48ON;       // Enable 48 MHz clock for RNG
   RNG->CR |= RNG_CR_RNGEN; // Enable RNG and interrupt
//   RNG->CR |= RNG_CR_CONDRST;
//   RNG->CR &= ~RNG_CR_CONDRST;
   RNG->SR |= RNG_SR_DRDY;                // Ensure RNG is ready
}

/*
 * Function 2/2:  rng
 * --------------------
 * produces random number, helper function
 *
 *	takes in: nothing
 *
 *  returns: random value from 0.. 4294967296
 */
uint32_t rng(void) {

    // Simple polling read with basic error handling
    // Assumes initialize_rng() enabled HSI48 + RNGEN
    // Clear any previous errors
    if (RNG->SR & (RNG_SR_CECS | RNG_SR_SECS)) {
        RNG->SR = 0;               // write to clear sticky error flags
//        RNG->CR |= RNG_CR_CONDRST; // recondition
//        RNG->CR &= ~RNG_CR_CONDRST;
    }

    // Wait for data ready (quick loop; add timeout if desired)
    while ((RNG->SR & RNG_SR_DRDY) == 0) {
        // optionally add a small software watchdog to avoid infinite loop
    }

    return RNG->DR;  // 32-bit random value
}
