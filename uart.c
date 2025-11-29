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
* project         : EE 329 Final Project
* authors         : Karina Wilson and Vanessa Guzman
* version         : 1
* date            : 11/25/2025
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
* 11/25/2025      : Updated table as leaderboard
******************************************************************************
*/

#include "uart.h"
#include "main.h"
#include "EEPROM.h"
//#include "delay.h"

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
void UART_setup(void)
{
   PWR->CR2 |= (PWR_CR2_IOSV);              // power avail on PG[15:2] (LPUART1)
   RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);   // enable GPIOG clock
   RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable LPUART clock bridge
   RCC ->CCIPR &= ~(RCC_CCIPR_LPUART1SEL_Msk);//used to select the correct clock
   RCC ->CCIPR |= RCC_CCIPR_LPUART1SEL_0;


   /* USER: configure GPIOG registers MODER/PUPDR/OTYPER/OSPEEDR then
      select AF mode and specify which function with AFR[0] and AFR[1] */
   //configuring PG7 and PG8 for alternate functions
   GPIOG -> MODER &=~(GPIO_MODER_MODE7_Msk | GPIO_MODER_MODE8_Msk);
   GPIOG -> MODER |=(GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1);
   GPIOG->OTYPER  &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT8);
   GPIOG->PUPDR   &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);
   GPIOG->PUPDR   |= GPIO_PUPDR_PUPD7_0;
   GPIOG->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED7_Pos) |
         (3 << GPIO_OSPEEDR_OSPEED8_Pos));


   //GPIOG->AFR[1]   |= (GPIOG ->AFR[1]&~ (0XFU <<(OU)) GPIO_AFRH_AFSEL8_2;
   //Set AF for PG7 (AFR[0], nibble at 7*4)
   GPIOG->AFR[0] &= ~(0X000F << GPIO_AFRL_AFSEL7_Pos);
   GPIOG->AFR[0] |=  (0X0008 << GPIO_AFRL_AFSEL7_Pos);
   GPIOG->AFR[1] &= ~(0X000F << GPIO_AFRH_AFSEL8_Pos);
   GPIOG->AFR[1] |=  (0X0008 << GPIO_AFRH_AFSEL8_Pos); // possibly 1


   LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); // 8-bit data
   LPUART1->CR1 |= USART_CR1_UE;                   // enable LPUART1
   LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);  // enable xmit & recv
   LPUART1->CR1 |= USART_CR1_RXNEIE;        // enable LPUART1 recv interrupt
   LPUART1->ISR &= ~(USART_ISR_RXNE);       // clear Recv-Not-Empty flag
   /* USER: set baud rate register (LPUART1->BRR) */
   LPUART1->BRR = 0x115C7;
   NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));   // enable LPUART1 ISR
   __enable_irq();                          // enable global interrupts
}


/************************************************************
 * Function: LPUART_Print()
 * Purpose : serial transmission using LPUART1 single character
 * Returns : None
 * Notes   : - sends one character at a time
************************************************************/
void LPUART_Print( const char* message ) {
   uint16_t iStrIdx = 0;
   while ( message[iStrIdx] != 0 ) {
      while(!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty xmit buffer
         ;
      LPUART1->TDR = message[iStrIdx];       // send this character
      iStrIdx++;                             // advance index to next char
   }
}




/************************************************************
 * Function: LPUART_Print_string()
 * Purpose : Send string or sequence of characters of given length
 * Returns : None
 * Notes   : Used for formatted VT100 output or border drawing
 ************************************************************/
void LPUART_Print_string(const char* s_message, int length){
   /* used to send string one time through a for loop until null char*/
   if (length == 0){
      for (int i = 0; ; i++){
         char c = s_message[i];
         if (c == 0) break;
         LPUART_wait_transmit();
         LPUART1->TDR = (uint8_t)c;
      }
   } else {/*used for the horizontal printing of border, will iterate
    *through length number of times*/
     for (int i = 0; i < length; i++){
        LPUART_wait_transmit();
        LPUART1->TDR = (uint8_t)s_message[i];
      }
   }
   while (!(LPUART1->ISR & USART_ISR_TC)) {}
}
void LPUART_ESC_Print(const char* esc_msg)
{
   LPUART_wait_transmit();
   LPUART1 ->TDR = '\x1B';
   for(int indx = 0; ;indx++){
      if(esc_msg[indx] == 0)
         break;
      LPUART_wait_transmit();
      LPUART1->TDR =esc_msg[indx];
   }
}


/************************************************************
 * Function: LPUART_wait_transmit()
 * Purpose : Wait for UART transmit buffer to be ready
 * Returns : None
 * Notes   : Polls TXE flag before continuing
 ************************************************************/
void LPUART_wait_transmit(void)
{
   while(!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty xmit buffer
      ;
}



/************************************************************
 * Function: LPUART_Set_Cursor_Location()
 * Purpose : Move VT100 cursor to specific row and column
 * Returns : None
 * Notes   : Builds escape code for cursor positioning
 ************************************************************/
void LPUART_Set_Cursor_Location(uint8_t row, uint8_t col)
{
   //array position for the values row and col less than 10
   if(row<10 && col<10){
      //create char array to feed to print string
      char cursor_move_2_2[] = {'\x1B', '[',row+0x30, ';',col+0x30, 'H', 0x00};
      LPUART_Print_string(cursor_move_2_2,0);
   }
   //array position  row less than 10 column less than 100
   else if(row<10 && col<100){
      /*sort tens and ones position for each row to create proper array
       * for cursor location placement*/
      char ten_col = (char)('0' + ((col / 10) % 10));
      char one_col = (char)('0' + (col % 10));
      //create char array to feed to print string
      char cursor_move_2_2[] = {'\x1B', '[',row+0x30, ';',
            ten_col,one_col, 'H', 0x00};
      LPUART_Print_string(cursor_move_2_2,0);
   }
   //array position row less than 10 column less than 200
   else if(row<10 && col<200){
      /*sort tens and ones position for each row to create proper array
       * for cursor location placement*/
      char hun_col = (char)('0' + ((col / 100) % 10));
      char ten_col = (char)('0' + ((col / 10)  % 10));
      char one_col = (char)('0' + (col % 10));
      //create char array to feed to print string
      char cursor_move_2_2[] = {'\x1B', '[',row+0x30, ';',
            hun_col,ten_col,one_col, 'H', 0x00};
      LPUART_Print_string(cursor_move_2_2,0);
   }
   //array position row less than 100 column less than 100
   else if(row<100 && col<100){//used for maximized window setting
      /*sort tens and ones position for each row to create proper array
       * for cursor location placement*/
      char ten_row = (char)('0' + ((row / 10) % 10));
      char one_row = (char)('0' + (row % 10));
      char ten_col = (char)('0' + ((col / 10) % 10));
      char one_col = (char)('0' + (col % 10));
      //create char array to feed to print string
      char cursor_move_2_2[] = {'\x1B', '[',ten_row,one_row,
            ';',ten_col,one_col, 'H', 0x00};
      LPUART_Print_string(cursor_move_2_2,0);
   }
   //array position row less than 100 column less than 200
   else if(row<100 && col<200){ //used for maximized window setting
      /*sort tens and ones position for each row and hundred,tens,ones for
       * column location to create proper array for cursor location placement*/
      char ten_row = (char)('0' + ((row / 10) % 10));
      char one_row = (char)('0' + (row % 10));
      char hun_col = (char)('0' + ((col / 100) % 10));
      char ten_col = (char)('0' + ((col / 10)  % 10));
      char one_col = (char)('0' + (col % 10));
      //create char array to feed to print string
      char cursor_move_2_2[] = {'\x1B', '[',ten_row,one_row,
            ';',hun_col,ten_col,one_col, 'H', 0x00};
      LPUART_Print_string(cursor_move_2_2,0);
   }
}

/*
* Function 2/11:  draw_border
* ---------------------------------------------------------------------------
* Draws the entire rectangular game border using terminal graphics.
*   sets color, draws sides/corners using print_column, returns cursor home
*
* takes in: nothing
*
* returns: nothing
*/
//void draw_border(void) {
//  // Set cyan color and home
//  LPUART_ESC_Print("36m");
//  LPUART_ESC_Print("H");
//  // Draw vertical borders
//  for (int border_row = 1; border_row <= GAME_ROWS; border_row++) {
//	  LPUART_Set_Cursor_Location(border_row, 1);
//	  LPUART_Print_string("║", 0);
//
//	  LPUART_Set_Cursor_Location(border_row, GAME_COLS);
//	  LPUART_Print_string("║", 0);
//
//  }
//  // Draw horizontal borders
//  for (int border_col = 1; border_col <= GAME_COLS; border_col++) {
//	  LPUART_Set_Cursor_Location(1, border_col);
//	  LPUART_Print_string("═", 0);
//
//	  LPUART_Set_Cursor_Location(GAME_ROWS, border_col);
//	  LPUART_Print_string("═", 0);
//
//   }
//   // Draw corners
//  LPUART_Set_Cursor_Location(1, 1);
//  LPUART_Print_string("╔", 0);
//
//  LPUART_Set_Cursor_Location(1, GAME_COLS);
//  LPUART_Print_string("╗", 0);
//
//  LPUART_Set_Cursor_Location(GAME_ROWS, 1);
//  LPUART_Print_string("╚", 0);
//
//  LPUART_Set_Cursor_Location(GAME_ROWS, GAME_COLS);
//  LPUART_Print_string("╝", 0);
//}


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
	LPUART_ESC_Print("[2J");
	LPUART_ESC_Print("[H");

	LPUART_Set_Cursor_Location(10, 10);
	LPUART_Print_string("           ___________________________          ", 0);

	LPUART_Set_Cursor_Location(11, 10);
	LPUART_Print_string("         /\\                         /\\         ", 0);

	LPUART_Set_Cursor_Location(12, 10);
	LPUART_Print_string("        /  \\        *FLASH*        /  \\        ", 0);

	LPUART_Set_Cursor_Location(13, 10);
	LPUART_Print_string("       / /\\ \\                    / /\\ \\       ", 0);

	LPUART_Set_Cursor_Location(14, 10);
	LPUART_Print_string("      / /__\\ \\    REACTIONX     / /__\\ \\      ", 0);

	LPUART_Set_Cursor_Location(15, 10);
	LPUART_Print_string("     /_/____\\_\\_______________ /_/____\\_\\     ", 0);

	LPUART_Set_Cursor_Location(16, 10);
	LPUART_Print_string("     \\ \\    / /   TEST YOUR    \\ \\    / /     ", 0);

	LPUART_Set_Cursor_Location(17, 10);
	LPUART_Print_string("      \\ \\  / /   REFLEX SPEED   \\ \\  / /      ", 0);

	LPUART_Set_Cursor_Location(18, 10);
	LPUART_Print_string("       \\ \\/ /                    \\ \\/ /       ", 0);

	LPUART_Set_Cursor_Location(19, 10);
	LPUART_Print_string("        \\  /    > PRESS START <    \\  /        ", 0);

	LPUART_Set_Cursor_Location(20, 10);
	LPUART_Print_string("         \\/ _______________________ \\/         ", 0);

	LPUART_Set_Cursor_Location(21, 10);
	LPUART_Print_string("            Cal Poly E329 - Team LEDZ         ", 0);


//
//	  delay_us(4000000);
//	  delay_us(1000000);
//
//
//	  LPUART_ESC_Print("[2J");   // Clear screen
//	  LPUART_ESC_Print("[H");    // Move to home position
//	  draw_border();              // Draw the border
//	  LPUART_ESC_Print("[H");    // Reset to home again
//	  LPUART_ESC_Print("[12B");  // Move down 12 lines
//	  LPUART_ESC_Print("[40C");  // Move right 40 columns
//	  //LPUART1_print("O");         // Print the O

}

void waitForStart(void)
{
    LPUART_Print("\r\nPress any key to start...\r\n");

    // Wait until a character is received
    while (!(LPUART1->ISR & USART_ISR_RXNE));
    (void)LPUART1->RDR; // read it (doesn’t matter what key)
    LPUART_ESC_Print("[2J");
    LPUART_ESC_Print("[H");

}

void getInitials(char *name)
{
    uint8_t count = 0;
    char c;

    LPUART_Print("\r\nEnter your initials (3 letters): ");

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
    }

    // No null terminator needed if NAME_LEN = 3
    LPUART_Print("\r\n");
}



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
    // Top border (row 12)
    LPUART_ESC_Print("[12;27H");
    for (int i = 0; i < 24; i++)
        LPUART_Print_string("\xE2\x95\x90", 0);

    // Bottom border (row 36)
    LPUART_ESC_Print("[36;27H");
    for (int i = 0; i < 24; i++)
        LPUART_Print_string("\xE2\x95\x90", 0);

    // Vertical borders
    for (uint8_t r = 13; r <= 36; r++)
    {
        LPUART_Set_Cursor_Location(r, 27);  // left border
        LPUART_Print_string("\xE2\x95\x91", 0);

        LPUART_Set_Cursor_Location(r, 51);  // right border
        LPUART_Print_string("\xE2\x95\x91", 0);
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
    const char *TL = "\xE2\x95\x94";
    const char *TR = "\xE2\x95\x97";
    const char *BL = "\xE2\x95\x9A";
    const char *BR = "\xE2\x95\x9D";

    LPUART_Set_Cursor_Location(12, 27);
    LPUART_Print_string(TL, 0);

    LPUART_Set_Cursor_Location(12, 51);
    LPUART_Print_string(TR, 0);

    LPUART_Set_Cursor_Location(36, 27);
    LPUART_Print_string(BL, 0);

    LPUART_Set_Cursor_Location(36, 51);
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
    // Separators at rows 15, 17, 19, 21, 23
    for (uint8_t row = 14; row <= 36; row += 2)
    {
        LPUART_Set_Cursor_Location(row, 27);
        LPUART_Print_string("\xE2\x95\xA0", 0);  // ╠

        for (int i = 0; i < 24; i++)
            LPUART_Print_string("\xE2\x95\x90", 0);  // ═

        LPUART_Set_Cursor_Location(row, 51);
        LPUART_Print_string("\xE2\x95\xA3", 0);  // ╣
    }
}

//converts int to string value to be used by Print_string()
void uint_to_str(uint16_t value, char *buf)
{
    char temp[6];    // temporary backwards buffer
    uint8_t idx = 0;

    // Extract digits in reverse order
    do {
        temp[idx++] = (value % 10) + '0';  // convert digit to ASCII
        value /= 10;
    } while (value > 0);

    // Reverse into the output buffer
    for (uint8_t i = 0; i < idx; i++) {
        buf[i] = temp[idx - 1 - i];
    }

    buf[idx] = 0;  // null terminator — MAKE IT A STRING
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
   LPUART_Print_string("==LEADERBOARD==",0);
   LPUART_Set_Cursor_Location(15,29);
   LPUART_Print_string("Rank",0);
   LPUART_Set_Cursor_Location(15,35);
   LPUART_Print_string("Name",0);
   LPUART_Set_Cursor_Location(15,43);
   LPUART_Print_string("Score",0);
   uint8_t count = 10;
   uint8_t i;
   for(i=0; i<count; i++){
	   char buf[6];
	   uint_to_str(i+1, buf);      // manual number→string function
	   LPUART_Set_Cursor_Location(17 + (i*2), 29);
	   LPUART_Print_string(buf, 0); //print rankings 1-10
	   char name_str[4];

	   //print initials
	   name_str[0] = leaderboard[i].name[0];
	   name_str[1] = leaderboard[i].name[1];
	   name_str[2] = leaderboard[i].name[2];
	   name_str[3] = 0;               // null-terminate
	   LPUART_Set_Cursor_Location(17 + (i*2), 35);
	   LPUART_Print_string(name_str, 0);

	   //print scores
	   uint_to_str(leaderboard[i].score, buf);   // convert score → string
	   LPUART_Set_Cursor_Location(17 + (i*2), 43);
	   LPUART_Print_string(buf, 0);

   }


}







