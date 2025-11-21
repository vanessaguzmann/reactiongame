
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
void setup_LPUART1(void);
void LPUART1_Print( const char* message );
void LPUART1_ESC_Print( const char* message );
void LPUART1_ESC_Print_Deliverable(void);
void LPUART1_IRQHandler( void  );

#endif // UART_H
