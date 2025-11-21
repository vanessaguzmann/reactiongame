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
void LPUART1_Game_Setup(void) {
   // Move cursor down 3 lines and right 5 spaces
	LPUART1_ESC_print("[2J");
	LPUART1_ESC_print("[H");

	printColumn(10, 10, "          ___________________________          ");
	printColumn(11, 10, "         /\\                         /\\         ");
	printColumn(12, 10, "        /  \\        *FLASH*        /  \\        ");
	printColumn(13, 10, "       / /\\ \\                    / /\\ \\       ");
	printColumn(14, 10, "      / /__\\ \\    REACTIONX     / /__\\ \\      ");
	printColumn(15, 10, "     /_/____\\_\\_______________ /_/____\\_\\     ");
	printColumn(16, 10, "     \\ \\    / /   TEST YOUR    \\ \\    / /     ");
	printColumn(17, 10, "      \\ \\  / /   REFLEX SPEED   \\ \\  / /      ");
	printColumn(18, 10, "       \\ \\/ /                    \\ \\/ /       ");
	printColumn(19, 10, "        \\  /    > PRESS START <    \\  /        ");
	printColumn(20, 10, "         \\/_______________________  \\/         ");
	printColumn(21, 10, "            Cal Poly E329 - Team LEDZ         ");


	  delay_us(4000000);
	  delay_us(1000000);


	  LPUART1_ESC_print("[2J");   // Clear screen
	  LPUART1_ESC_print("[H");    // Move to home position
	  draw_border();              // Draw the border
	  LPUART1_ESC_print("[H");    // Reset to home again
	  LPUART1_ESC_print("[12B");  // Move down 12 lines
	  LPUART1_ESC_print("[40C");  // Move right 40 columns
	  //LPUART1_print("O");         // Print the O

}

void waitForStart(void)
{
    char c;

    LPUART1_Print("\r\nPress any key to start...\r\n");

    // Wait until a character is received
    while (!(LPUART1->ISR & USART_ISR_RXNE));
    c = LPUART1->RDR; // read it (doesn’t matter what key)
    LPUART1_ESC_Print("[2J");
    LPUART1_ESC_Print("[H");

}

void getInitials(char *name)
{
    uint8_t count = 0;
    char c;

    LPUART1_Print("\r\nEnter your initials (3 letters): ");

    while (count < 3)
    {
        // Wait until data is received
        while (!(LPUART1->ISR & USART_ISR_RXNE));
        c = LPUART1->RDR;  // read received byte

        // Only accept A–Z
        if (c >= 'A' && c <= 'Z')
        {
            name[count++] = c;
            LPUART1->TDR = c; // echo character back to terminal
            while (!(LPUART1->ISR & USART_ISR_TXE));

    }

    // No null terminator needed if NAME_LEN = 3
    LPUART1_Print("\r\n");
}


typedef struct {
    char name[NAME_LEN];
    uint16_t score;
} Player;

/************************************************************
 * Function: A8_Extra_Credit_Table()
 * Purpose : Draw formatted ADC data table on terminal
 * Returns : None
 * Notes   : Calls helper functions to draw borders and labels
 ************************************************************/
void A8_Extra_Credit_Table(void)
{
   //drawing the borders of the table
   A8_ADC_Chart_Borders();
   LPUART_Draw_Corners();
   LPUART_Draw_Inner_Ends();
   LPUART_Chart_Words();
}


/************************************************************
 * Function: A8_ADC_Chart_Borders()
 * Purpose : Draw top, bottom, and vertical chart borders
 * Returns : None
 * Notes   : Used for VT100 box-drawing of ADC chart
 ************************************************************/
void A8_ADC_Chart_Borders(void)
{
   /*chart starts  at row 12 col227, and goes to row 22 col 27 vertically
    * the chart goes horizontally from col 20 to column 41*/
   //Drawing top and bottom borders
   LPUART_ESC_Print("[12;27H");                 // home (row 1, col 1)
   for (int i = 0; i < 24; i++)           // top border
      LPUART_Print_string("\xE2\x95\x90", 0);
   LPUART_ESC_Print("[22;27H");             // bottom-left corner (row 99)
   for (int i = 0; i < 24; i++)           // bottom border
      LPUART_Print_string("\xE2\x95\x90", 0);
   //Drawing the vertical, left and right borders
   // Left vertical at column 19
   for (uint8_t r = 1; r <= 9; r++){
      LPUART_Set_Cursor_Location(r+12, 27);
      LPUART_Print_string("\xE2\x95\x91", 0);   //prints left border ║
   }
   // Right vertical at column 61
   for (uint8_t r = 1; r <= 9; r++){
      LPUART_Set_Cursor_Location(r+12, 51);
      LPUART_Print_string("\xE2\x95\x91", 0);   //prints right border ║
   }


   //This will draw the inner part of the table, will be placed in for loops
   for(int irow = 0;irow<8;irow +=2){
      LPUART_Set_Cursor_Location(irow+14,27);
      for (int i = 0; i < 24; i++)           //
         LPUART_Print_string("\xE2\x95\x90", 0);
   }
}


/************************************************************
 * Function: LPUART_Draw_Corners()
 * Purpose : Draw double-line UTF-8 box corners
 * Returns : None
 * Notes   : Positions cursor and prints corner glyphs
 ************************************************************/
void LPUART_Draw_Corners(void)
{
   // UTF-8 double-line corner glyphs
   const char *TL = "\xE2\x95\x94";  // ╔ top left corner
   const char *TR = "\xE2\x95\x97";  // ╗ top right corner
   const char *BL = "\xE2\x95\x9A";  // ╚ bottom left corner
   const char *BR = "\xE2\x95\x9D";  // ╝ bottom right corner


   // Top-left corner of border
   LPUART_Set_Cursor_Location(12, 27);
   LPUART_Print_string(TL, 0);


   // Top-right corner of border
   LPUART_Set_Cursor_Location(12, 51);
   LPUART_Print_string(TR, 0);


   // Bottom-left corner of border
   LPUART_Set_Cursor_Location(22, 27);
   LPUART_Print_string(BL, 0);


   // Bottom-right corner of border
   LPUART_Set_Cursor_Location(22, 51);
   LPUART_Print_string(BR, 0);
}


/************************************************************
 * Function: LPUART_Draw_Inner_Ends()
 * Purpose : Draw inner divisions and intersections in table
 * Returns : None
 * Notes   : Creates vertical separators and double-line joins
 ************************************************************/
void LPUART_Draw_Inner_Ends(void)
{
   /*The code will draw a double vertical to complete the table
    * E2 95 A0 ╠ Double vertical and right*/
   for(int irow = 0;irow<8;irow +=2){
      LPUART_Set_Cursor_Location(irow+14,27);
      LPUART_Print_string("\xE2\x95\xA0", 0);
      }
   /*This code will draw the double vertical to complete the right side
    * of the table with code: E2 95 A3  ╣ Double vertical and left*/
   for(int irow = 0;irow<8;irow +=2){
      LPUART_Set_Cursor_Location(irow+14,51);
      LPUART_Print_string("\xE2\x95\xA3", 0);
        }
   /*This code will draw the first inner chart verticals to separate the first
    * column of the table: this will start at row 13 column 25 and end at
    * row 20 column 25*/
      // Left vertical at column 33
      for (uint8_t r = 1; r <= 7; r++){
         LPUART_Set_Cursor_Location(r+12,33);
         LPUART_Print_string("\xE2\x95\x91", 0);   //prints inner border ║
      }
      // Left vertical at column 41
      for (uint8_t r = 1; r <= 7; r++){
         LPUART_Set_Cursor_Location(r+12,42);
         LPUART_Print_string("\xE2\x95\x91", 0);   //prints inner border ║
      }


      /*This code will draw the first inner chart  E2 95 AC ╬ Double vertical
       *  and horizontal that will separate the cells*/
      for(int irow = 0;irow<6;irow +=2){
         LPUART_Set_Cursor_Location(irow+14,33);
         LPUART_Print_string("\xE2\x95\xAC", 0);
      }
      for(int irow = 0;irow<6;irow +=2){
         LPUART_Set_Cursor_Location(irow+14,42);
         LPUART_Print_string("\xE2\x95\xAC", 0);
      }
      /*This code will draw E2 95 A6 ╦ Double down and horizontal for
       * cell separation*/
      LPUART_Set_Cursor_Location(12, 33);
      LPUART_Print_string("\xE2\x95\xA6", 0);
      LPUART_Set_Cursor_Location(12, 42);
      LPUART_Print_string("\xE2\x95\xA6", 0);
      /*This code will draw E2 95 A9 ╩ Double up and horizontal for
       * cell separation*/
      LPUART_Set_Cursor_Location(20, 33);
      LPUART_Print_string("\xE2\x95\xA9", 0);
      LPUART_Set_Cursor_Location(20, 42);
      LPUART_Print_string("\xE2\x95\xA9", 0);
}


/************************************************************
 * Function: LPUART_Chart_Words()
 * Purpose : Print text labels inside ADC chart
 * Returns : None
 * Notes   : Places labels such as “ADC”, “Counts”, and “Volts”
 ************************************************************/
void LPUART_Chart_Words(void)
{
/*This code will fill in the table with the measurement names needed*/
   LPUART_Set_Cursor_Location(13,29);
   LPUART_Print_string("ADC",0);
   LPUART_Set_Cursor_Location(13,35);
   LPUART_Print_string("Counts",0);
   LPUART_Set_Cursor_Location(13,44);
   LPUART_Print_string("Volts",0);
   LPUART_Set_Cursor_Location(15,29);
   LPUART_Print_string("MIN",0);
   LPUART_Set_Cursor_Location(17,29);
   LPUART_Print_string("MAX",0);
   LPUART_Set_Cursor_Location(19,29);
   LPUART_Print_string("AVG",0);
   LPUART_Set_Cursor_Location(21,29);
   LPUART_Print_string("Coil Current = ",0);
}







