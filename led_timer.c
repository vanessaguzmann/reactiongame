/**
  ******************************************************************************
  * @file           : led_timer.c
  * @brief          : led timer program body
  * @editors		: Vanessa G
  ******************************************************************************
  * * wiring    	: PC8-12 connected to 220ohm resistors, LEDS lead to GND
  * * attachment	: LED1-5 attached to PC8-12, respectively
  * 					1: white
  * 					2: yellow
  * 					3: green
  * 					4: blue
  * 					5: red
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
  * 10/08/25	Created file
  * 11/19/25	Removed nibble count from V1
  * 			Included delay and moved software delay
  ******************************************************************************
*/

#include "led_timer.h"
#include "delay.h"
#include "main.h"

/*
 * Function 1/DD:  led_init
 * --------------------
 * enables GPIOC clock, sets PC8..PC12 as output, push-pull, no pull, high speed
 *    and ensures all LEDS are off
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
void led_init(void) {
	// Enable GPIOC clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	// PC8..PC12 as output, push-pull, no pull, high speed
	GPIOC->MODER   &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9 |
	                    GPIO_MODER_MODE10 | GPIO_MODER_MODE11 |
						GPIO_MODER_MODE12);
	GPIOC->MODER   |=  (GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0 |
	                    GPIO_MODER_MODE10_0 | GPIO_MODER_MODE11_0 |
	                    GPIO_MODER_MODE12_0);
	GPIOC->OTYPER  &= ~(GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9 |
	                    GPIO_OTYPER_OT10 | GPIO_OTYPER_OT11 |
						GPIO_OTYPER_OT12);
	GPIOC->PUPDR   &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9 |
	                    GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD11 |
						GPIO_PUPDR_PUPD12);
	GPIOC->OSPEEDR |=  (3u << GPIO_OSPEEDR_OSPEED8_Pos) |
	                   (3u << GPIO_OSPEEDR_OSPEED9_Pos) |
	                   (3u << GPIO_OSPEEDR_OSPEED10_Pos)|
	                   (3u << GPIO_OSPEEDR_OSPEED11_Pos)|
					   (3u << GPIO_OSPEEDR_OSPEED12_Pos);

	// Ensure all LEDS are off
	GPIOC->BRR = LED_MASK;
}

/*
 * Function 2/DD:  flash_sequence
 * --------------------
 * turns on an LED randomly
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
void flash_sequence(void) {
   // Declare ranges with enums!
   enum { WHITE_START  = 0x00000000u, WHITE_END  = 0x33333333u }; // 0 .. 858,993,459
   enum { YELLOW_START = 0x33333334u, YELLOW_END = 0x66666666u }; // 858,993,460 .. 1,717,986,918
   enum { GREEN_START  = 0x66666667u, GREEN_END  = 0x99999999u }; // 1,717,986,919 .. 2,576,980,377
   enum { BLUE_START   = 0x9999999Au, BLUE_END   = 0xCCCCCCCCu }; // 2,576,980,378 .. 3,435,973,836
   enum { RED_START    = 0xCCCCCCCDu, RED_END    = 0xFFFFFFFFu }; // 3,435,973,837 .. 4,294,967,295

   // Make a random number generate
   uint32_t random_num = rng();

   // Turn on LED if in LEDS range
   switch ( random_num ) {
      case WHITE_START ... WHITE_END:
	     // Turn on all LEDS
		 GPIOC->BSRR = GPIO_PIN_8;
		 // Call a visible delay
		 software_delay(1500);
		 // Ensure all LEDS are off
		 GPIOC->BRR = GPIO_PIN_8;
		 // Call a visible delay
		 software_delay(1500);
	     break;
      case YELLOW_START ... YELLOW_END:
		 GPIOC->BSRR = GPIO_PIN_9;
		 // Call a visible delay
		 software_delay(1500);
		 // Ensure all LEDS are off
		 GPIOC->BRR = GPIO_PIN_9;
		 // Call a visible delay
		 software_delay(1500);
         break;
      case GREEN_START ... GREEN_END:
		 GPIOC->BSRR = GPIO_PIN_10;
		 // Call a visible delay
		 software_delay(1500);
		 // Ensure all LEDS are off
		 GPIOC->BRR = GPIO_PIN_10;
		 // Call a visible delay
		 software_delay(1500);
	     break;
      case BLUE_START ... BLUE_END:
		 GPIOC->BSRR = GPIO_PIN_11;
		 // Call a visible delay
		 software_delay(1500);
		 // Ensure all LEDS are off
		 GPIOC->BRR = GPIO_PIN_11;
		 // Call a visible delay
		 software_delay(1500);
	     break;
      case RED_START ... RED_END:
		 GPIOC->BSRR = GPIO_PIN_12;
		 // Call a visible delay
		 software_delay(1500);
		 // Ensure all LEDS are off
		 GPIOC->BRR = GPIO_PIN_12;
		 // Call a visible delay
		 software_delay(1500);
	     break;
   }
}

/*
 * Function X/DD:  timing_execution
 * --------------------
 * turns on all LEDS, for debugging
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
void flash_led(void) {
	// Turn on all LEDS
	GPIOC->BSRR = LED_MASK;

	// Call a visible delay
	software_delay(1500);					// is software delay needed

	// Ensure all LEDS are off
	GPIOC->BRR = LED_MASK;

	// Call a visible delay
	software_delay(1500);					// is software delay needed
}

/*
 * Function X/DD:  timing_execution
 * --------------------
 * turns on LED1, calls for the function, waits until the function execution
 *   is over, then turns off LED1, for debugging
 *
 *	takes in: a function call who has an integer as its parameter
 *
 *  *test_function: pointer that identifies a function with the same name,
 *     the (var_type) requires the function to have a var_type as the param
 *
 *  returns: nothing
 */
void timing_execution(var_type (*test_function)(var_type)) {
	//Ensure all LEDS are off
	GPIOC->BRR = LED_MASK;

	// Call a visible delay
	software_delay(1500);					// is software delay needed

	GPIOC->BSRR = (GPIO_PIN_8);             // turn on PC0

	// call test function and use 15 as parameter
	test_function(15);
	GPIOC->BRR = (GPIO_PIN_8);              // turn off PC0
}

