
/*
------------------------------------------------------------------------------
uart.h
------------------------------------------------------------------------------
* USER CODE BEGIN Header
******************************************************************************
* EE 329 uart.h
******************************************************************************
* @file           : uart.h
* @brief          : header for uart.c
* project         : EE 329 A2
* authors         : Vanessa G
* version         : 1
* date            : 10/22/2025
* compiler        : STM32CubeIDE v.1.19.0
* target          : NUCLEO-L4A6ZG
* clocks          : 4 MHz MSI to AHB2
* wiring       	  : PG7 as TX and PG8 as RX
* attachment	  : n/a
* @attention      : (c) 2023 STMicroelectronics.  All rights reserved.
******************************************************************************
* REVISION HISTORY
******************************************************************************
* 10/22/2025      :	Created file
******************************************************************************
*/

// -------------------------------------------------- #includes for uart.c --
#ifndef UART_H
#define UART_H

#include "stm32l4xx_hal.h"

// ---------- Function Prototypes --------------------------------------------
void UART_setup(void);
void LPUART_Print( const char* message );
void LPUART_Print_string(const char* s_message, int length);
void LPUART_ESC_Print(const char* esc_msg);
void LPUART_wait_transmit(void);
void LPUART_Set_Cursor_Location(uint8_t row, uint8_t col);
void draw_border(void);
void LPUART1_Game_Setup(void);
void waitForStart(void);
void getInitials(char *name);
void A8_Extra_Credit_Table(void);
void A8_ADC_Chart_Borders(void);
void LPUART_Draw_Corners(void);
void LPUART_Draw_Inner_Ends(void);
void uint_to_str(uint16_t value, char *buf);
void LPUART_Chart_Words(void);


#endif // UART_H
