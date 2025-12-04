/*
------------------------------------------------------------------------------
led_timer.h
------------------------------------------------------------------------------
* USER CODE BEGIN Header
******************************************************************************
* EE 329 nvic.h
******************************************************************************
* @file           : led_timer.h
* @brief          : led operations body
* project         : EE 329 Final Project
* authors         : Vanessa G
* version         : 1
* date            : 11/21/2025
* compiler        : STM32CubeIDE v.1.19.0
* target          : NUCLEO-L4A6ZG
* clocks          : 4 MHz MSI to AHB2
* wiring       	  : PC8, 9, 10, 11, 12
* attachment	  : 560 ohm resistors to cathode of LEDS
* @attention      : (c) 2023 STMicroelectronics.  All rights reserved.
******************************************************************************
* REVISION HISTORY
******************************************************************************
* 11/23/2025      :	Created file
******************************************************************************
*/

// ----------------------------------------------- #includes for led_timer.c -

#ifndef LED_TIMER_H
#define LED_TIMER_H

#include "stm32l4xx_hal.h"
#include <stdint.h>      // for uint32_t an more
#include <math.h>		 // for math functions
#include <stdbool.h>

// ---------- Defines --------------------------------------------------------
#define LED_MASK (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);

#define MAX_SEQ_LEN 32
extern volatile uint32_t sw_delay_ms;

// ---------- Custom Array Structure -----------------------------------------
typedef struct {
    uint8_t data[MAX_SEQ_LEN];
    uint32_t length;
} Sequence;

// ---------- Function Prototypes --------------------------------------------
void led_init(void);     // enables GPIOC, config PC8..PC11 outputs, LEDs off
void flash_led(void);			// turn every LED on
uint32_t flash_rnd_led(void);	// turns on random LED
void test_servo(void);			// turns on servo on and off
void Sequence_Init(Sequence *seq); // initialize sequence variable type
uint8_t Sequence_Append(Sequence *seq, uint8_t value);
void generate_led_sequence(Sequence *seq);
void show_sequence(const Sequence *seq);
void run_reaction_game(void);

#endif // LED_TIMER_H
