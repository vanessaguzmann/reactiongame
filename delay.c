/*
-----------------------------------------------------------------------------------
delay.c
-----------------------------------------------------------------------------------
* USER CODE BEGIN Header
******************************************************************************
* EE 329 delay.c
******************************************************************************
* @file           : delay.c
* @brief          :
* project         : EE 329 A3
* authors         : Vanessa Guzman
* version         : 1
* date            : 10/08/2025
* compiler        : STM32CubeIDE v.1.19.0
* target          : NUCLEO-L4A6ZG
* clocks          : 4 MHz MSI to AHB2
* @attention      : (c) 2023 STMicroelectronics.  All rights reserved.
******************************************************************************
* REVISION HISTORY
* 10/08/25	Created file
******************************************************************************
*/

#include "delay.h"

// --------------------------------------------------- delay.c w/o #includes ---
// configure SysTick timer for use with delay_us().
// warning: breaks HAL_delay() by disabling interrupts for shorter delay timing.
void SysTick_Init(void) {
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |     	// enable SysTick Timer
                      SysTick_CTRL_CLKSOURCE_Msk); 	// select CPU clock
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);  	// disable interrupt
}

// delay in microseconds using SysTick timer to count CPU clock cycles
// do not call with 0 : error, maximum delay.
// careful calling with small nums : results in longer delays than specified:
//	   e.g. @4MHz, delay_us(1) = 10-15 us delay.
void delay_us(const uint32_t time_us) {
	// set the counts for the specified delay
	SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
	SysTick->VAL = 0;                                  	 // clear timer count
	SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);    	 // clear count flag
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for flag
}

/*
 * Function 3/DD:  software_delay
 * --------------------
 * empty for loop
 *
 *	takes in: an integer of the desired time
 *
 *	desired_time: the parameter
 *
 *  loop_count: the number of loops the empty for loop has iterated through
 *    increases by one after every iteration
 *
 *  returns: nothing
 */
void software_delay(int desired_time) {
	for(volatile int loop_count=0; loop_count < desired_time * 100; loop_count++){
		// Empty for loop to create software delay
		;
	}
}
