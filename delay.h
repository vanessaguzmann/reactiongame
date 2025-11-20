/*
-----------------------------------------------------------------------------------
delay.h
-----------------------------------------------------------------------------------
* USER CODE BEGIN Header
******************************************************************************
* EE 329 delay.h
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

// --------------------------------------------------- #includes for delay.c ---

#ifndef DELAY_H
#define DELAY_H

#include "stm32l4a6xx.h"
#include <stdint.h>      // for uint32_t an more

// --- Function Prototypes ---
void SysTick_Init(void);
void delay_us(const uint32_t time_us);
void software_delay(int desired_time);
//void delay_ms(uint32_t ms);

#endif // DELAY_H
