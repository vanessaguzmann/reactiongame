/*
------------------------------------------------------------------------------
uart.c
------------------------------------------------------------------------------
* USER CODE BEGIN Header
******************************************************************************
* EE 329 uart.c
******************************************************************************
* @file           : uart.c
* @brief          : lpuart program body
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

#include "uart.h"
#include "main.h"

/*
 * Function 1/5:  setup_LPUART1
 * ---------------------------------------------------------------------------
 * enables GPIOG clock, sets PG7 and PG8 as AF8, push-pull, no pull,
 *    high speed
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
void setup_LPUART1(void) {
   // Power and peripheral clocks
   PWR->CR2 |= (PWR_CR2_IOSV);
   RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);
   RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
   // GPIOG Pin configuration for LPUART1
   // Set PG7 (TX) and PG8 (RX) to Alternate Function mode (AF8)
   GPIOG->MODER &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE8);
   GPIOG->MODER |= (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1);
   // Configure pull-up on RX pin (PG8), no pull for TX pin (PG7)
   GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);
   GPIOG->PUPDR |= (GPIO_PUPDR_PUPD8_0);
   // Set both pins to high speed
   GPIOG->OSPEEDR |= (GPIO_OSPEEDR_OSPEED7) | (GPIO_OSPEEDR_OSPEED8);
   // Set AF8 for PG7 and PG8 in AFR registers
   GPIOG->AFR[0] &= ~(0x000F << GPIO_AFRL_AFSEL7_Pos);
   GPIOG->AFR[1] &= ~(0x000F << GPIO_AFRH_AFSEL8_Pos);
   GPIOG->AFR[0] |= (0x0008 << GPIO_AFRL_AFSEL7_Pos);
   GPIOG->AFR[1] |= (0x0008 << GPIO_AFRH_AFSEL8_Pos);
   // LPUART1 Peripheral configuration
   LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0);
   LPUART1->CR1 |= USART_CR1_UE;
   LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);
   // Enable "receive data register not empty" interrupt
   // Clear RXNE flag
   LPUART1->CR1 |= USART_CR1_RXNEIE;
   LPUART1->ISR &= ~(USART_ISR_RXNE);
   // Set baud rate (BRR). Value 8888
   LPUART1->BRR = 8888;
   // Interrupt configuration
   NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));
   __enable_irq();
}

/*
 * Function 2/5:  LPUART_Print
 * ---------------------------------------------------------------------------
 * checks if the transmit buffer is empty and writes character into data
 *   transmit register. repeats process byte-by-byte of string. prints
 *   string!
 *
 *	takes in: constant char* message, a string
 *
 *  returns: nothing
 */
void LPUART1_Print( const char* message ) {
   uint16_t iStrIdx = 0;
   while ( message[iStrIdx] != 0 ) {
      while(!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty xmit buffer
      ;
      LPUART1->TDR = message[iStrIdx];       // send this character
	  iStrIdx++;                             // advance index to next char
   }
}

/*
 * Function 3/5:  LPUART1_ESC_Print
 * ---------------------------------------------------------------------------
 * checks if the transmit buffer is empty and writes character into data
 *   transmit register. repeats process byte-by-byte of string. prints
 *   string!
 *
 *	takes in: constant char* message, a string
 *
 *  returns: nothing
 */
void LPUART1_ESC_Print( const char* message ) {
   const char ESC = 0x1B;  // ASCII Escape character
   // Send ESC character and '[' prefix via polling
   while (!(LPUART1->ISR & USART_ISR_TXE));
   LPUART1->TDR = ESC;
   while (!(LPUART1->ISR & USART_ISR_TXE));
   LPUART1->TDR = '[';
   LPUART1_Print(message);
}

/*
 * Function 4/5:  LPUART1_ESC_Print_Deliverable
 * ---------------------------------------------------------------------------
 * checks if the transmit buffer is empty and writes character into data
 *   transmit register. repeats process byte-by-byte of string. prints
 *   string!
 *
 *	takes in: constant char* message, a string
 *
 *  returns: nothing
 */
void LPUART1_ESC_Print_Deliverable(void) {
   // Move cursor down 3 lines and right 5 spaces
   LPUART1_ESC_Print("3B");
   LPUART1_ESC_Print("5C");

   // Print text
   LPUART1_Print("All good students read the");

   // Move cursor down 1 line and left 21 spaces
   LPUART1_ESC_Print("1B");
   LPUART1_ESC_Print("21D");

   // Enable blinking text
   LPUART1_ESC_Print("5m");

   // Print blinking text
   LPUART1_Print("Reference Manual");

   // Move cursor to top-left corner
   LPUART1_ESC_Print("H");

   // Disable blinking (reset attributes)
   LPUART1_ESC_Print("0m");

   // Print final text
   LPUART1_Print("Input: ");
}

/*
 * Function 5/5:  LPUART_IRQHandler
 * ---------------------------------------------------------------------------
 * handles received characters from terminal with interrupt to change
 *   direction
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
void LPUART1_IRQHandler( void  ) {
   uint8_t charRecv;
   // Check if the receive-data-register-not-empty (RXNE) flag is set
   if (LPUART1->ISR & USART_ISR_RXNE) {
	  // Read received character (automatically clears RXNE flag)
	  charRecv = LPUART1->RDR;

	  switch ( charRecv ) {
	  // Handle color change commands and call ESC print wrapper
	     case 'R':
		    LPUART1_ESC_Print("31m");
		    break;
	     case 'B':
		    LPUART1_ESC_Print("34m");
		    break;
	     case 'G':
	    	LPUART1_ESC_Print("32m");
	    	break;
	     case 'W':
	    	LPUART1_ESC_Print("37m");
	    	break;

//	  // Handle direction change commands
//	     case 'w':
//	    	input_flags |= FLAG_DIR_UP;
//	    	break;
//	     case 's':
//	    	input_flags |= FLAG_DIR_DOWN;
//	    	break;
//	     case 'd':
//	    	input_flags |= FLAG_DIR_RIGHT;
//	    	break;
//	     case 'a':
//	    	input_flags |= FLAG_DIR_LEFT;
//	    	break;

	  // Default case
	  default:
	     while( !(LPUART1->ISR & USART_ISR_TXE) )
            ;    // wait for empty TX buffer
	     LPUART1->TDR = charRecv;
	  }
   }
}


