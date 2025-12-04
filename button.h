/*
------------------------------------------------------------------------------
buttons.h
------------------------------------------------------------------------------
* USER CODE BEGIN Header
******************************************************************************
* EE 329 buttons.h
******************************************************************************
* @file           : buttons.h
* @brief          : buttons program body
* project         : EE 329 A2
* authors         : Vanessa G
* version         : 1
* date            : 11/21/2025
* compiler        : STM32CubeIDE v.1.19.0
* target          : NUCLEO-L4A6ZG
* clocks          : 4 MHz MSI to AHB2
* wiring       	  : PB13, 12, 4, 5, 3
* attachment	  : n/a
* @attention      : (c) 2023 STMicroelectronics.  All rights reserved.
******************************************************************************
* REVISION HISTORY
******************************************************************************
* 11/21/2025      :	Created file
******************************************************************************
*/

// ----------------------------------------------- #includes for buttons.c ---

#ifndef BUTTONS_H
#define BUTTONS_H

// ---------- Defines ----------------------------------------------------------
#include "stm32l4xx_hal.h"

#define BUTTON_PORT GPIOB

#define WHITE_BUTTON GPIO_PIN_3
#define YELLOW_BUTTON GPIO_PIN_5
#define GREEN_BUTTON GPIO_PIN_4
#define BLUE_BUTTON GPIO_PIN_12
#define RED_BUTTON GPIO_PIN_13
#define ALL_BUTTON_PINS (WHITE_BUTTON | YELLOW_BUTTON | GREEN_BUTTON | BLUE_BUTTON | RED_BUTTON)

#define BIT0 0x01
#define SETTLE 10000

#define NO_PRESS 0

// ---------- Button Color Code Variables ------------------------------------------
// Encoded return values for which button is pressed
#define WHITE_CODE      1
#define YELLOW_CODE     2
#define GREEN_CODE      3
#define BLUE_CODE       4
#define RED_CODE        5

// ---------- Function Prototypes ----------------------------------------------
void buttons_init(void);
int  buttons_IsAnyButtonPressed(void);
int  buttons_WhichButtonIsPressed(void);
int read_user_color_until(uint32_t deadline_ms, uint32_t *color_out);
int level_up(void);

// ---------- Interrupt Function Prototypes -------------------------------------
void buttons_exti_init(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

// -------- Global flags (defined here, declared extern in header) ------------
extern volatile uint8_t g_button_pressed_flag;
extern volatile uint8_t g_button_color_flag;
extern volatile uint8_t g_button_event_ready;

#endif // BUTTON_H

