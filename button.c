/*
------------------------------------------------------------------------------
buttons.c
------------------------------------------------------------------------------
* USER CODE BEGIN Header
******************************************************************************
* EE 329 buttons.c
******************************************************************************
* @file           : buttons.c
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

#include "buttons.h"
#include "main.h"

volatile uint8_t g_button_pressed_flag = 0;
volatile uint8_t g_button_color_flag   = 0;
volatile uint8_t g_button_event_ready  = 0;  // 1 = new button event consumed

/*
 * Function 1:  buttons_init
 * --------------------
 * enables GPIOB clock,
 * 	  sets PB3, PB5, PB4, PB12, PB13 as input, push-pull,
 * 	  pull down, high speed
 * logic: pressed- pin reads 1 (due to ext pull-down)
 *   un-pressed- pin reads 0 (connected to GND)
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
void buttons_init(void) {
   // Enable GPIOB clock
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

   // PB3, PB5, PB4, PB12, PB13 as input, EXT pull down, high speed
   GPIOB->MODER   &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE5 |
		   	   	   	   GPIO_MODER_MODE4 | GPIO_MODER_MODE12 |
					   GPIO_MODER_MODE13);
   GPIOB->PUPDR   &= ~(GPIO_PUPDR_PUPD3 | GPIO_PUPDR_PUPD5 |
					   GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD12 |
					   GPIO_PUPDR_PUPD13);
   GPIOB->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED3 | GPIO_OSPEEDR_OSPEED5 |
					   GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED12 |
					   GPIO_OSPEEDR_OSPEED13);
}

/*
 * Function 2: buttons_IsAnyButtonPressed
 * --------------------
 * detects if any button was pressed
 *
 *	takes in: nothing
 *
 *  returns: if any press was detected
 *     1 = yes
 *     0 = no
 */
int buttons_IsAnyButtonPressed(void)
{
   return (BUTTON_PORT->IDR & ALL_BUTTON_PINS) ? 1 : 0;
}

/*
 * Function 3: buttons_WhichButtonIsPressed
 * --------------------
 * detects which button was pressed
 *
 *	takes in:
 *
 *  returns: encoded button code
 *   1 = white, 2 = yellow, 3 = green, 4 = blue, 5 = red
 *   0 = NO_PRESS
 */
int buttons_WhichButtonIsPressed(void) {
   uint32_t idr = BUTTON_PORT->IDR;

   // Active-hi checks: if bit is 1, that button is pressed
   if (idr & WHITE_BUTTON)  return WHITE_CODE;
   if (idr & YELLOW_BUTTON) return YELLOW_CODE;
   if (idr & GREEN_BUTTON)  return GREEN_CODE;
   if (idr & BLUE_BUTTON)   return BLUE_CODE;
   if (idr & RED_BUTTON)    return RED_CODE;

   return NO_PRESS;
}

/*
 * Function 4: buttons_exti_init
 * --------------------
 * configure EXTI interrupts on PB3, PB4, PB5, PB12, PB13.
 *    active-high buttons => use rising-edge triggers.
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
void buttons_exti_init(void) {
   // Enable SYSCFG clock (needed for EXTI routing)
   RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

   // Map PB3, PB4, PB5, PB12, PB13 -> EXTI3,4,5,12,13 via SYSCFG_EXTICR

   // EXTI3 on PB3 -> EXTICR1[15:12]
   SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;  // clear
   SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI3_PB;

   // EXTI4 on PB4 -> EXTICR2[3:0]
   SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI4;
   SYSCFG->EXTICR[1] |=  SYSCFG_EXTICR2_EXTI4_PB;

   // EXTI5 on PB5 -> EXTICR2[7:4]
   SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI5;
   SYSCFG->EXTICR[1] |=  SYSCFG_EXTICR2_EXTI5_PB;

   // EXTI12 on PB12 -> EXTICR4[7:4]
   SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI12;
   SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI12_PB;

   // EXTI13 on PB13 -> EXTICR4[11:8]
   SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
   SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PB;

   // Enable interrupts in EXTI: IMR1
   EXTI->IMR1 |= (EXTI_IMR1_IM3  |
                  EXTI_IMR1_IM4  |
                  EXTI_IMR1_IM5  |
                  EXTI_IMR1_IM12 |
                  EXTI_IMR1_IM13);

   // 4) ACTIVE-HIGH: use RISING edge (0 -> 1)
   EXTI->RTSR1 |= (EXTI_RTSR1_RT3  |
                   EXTI_RTSR1_RT4  |
                   EXTI_RTSR1_RT5  |
                   EXTI_RTSR1_RT12 |
                   EXTI_RTSR1_RT13);

   // 5) Disable falling edge (optional but good hygiene)
   EXTI->FTSR1 &= ~(EXTI_FTSR1_FT3  |
                    EXTI_FTSR1_FT4  |
                    EXTI_FTSR1_FT5  |
                    EXTI_FTSR1_FT12 |
                    EXTI_FTSR1_FT13);


   // Enable NVIC interrupts for EXTI lines
   NVIC_EnableIRQ(EXTI3_IRQn);
   NVIC_SetPriority(EXTI3_IRQn, 2);

   NVIC_EnableIRQ(EXTI4_IRQn);
   NVIC_SetPriority(EXTI4_IRQn, 2);

   NVIC_EnableIRQ(EXTI9_5_IRQn);
   NVIC_SetPriority(EXTI9_5_IRQn, 2);

   NVIC_EnableIRQ(EXTI15_10_IRQn);
   NVIC_SetPriority(EXTI15_10_IRQn, 2);
}

/*
 * Function 5..8: Interrupt service routines
 * --------------------
 * each ISR:
 *  clears EXTI pending bit, sets g_button_pressed_flag = 1,
 *  sets g_button_color_flag = 1..5
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
// EXTI line 3 -> PB3 (WHITE)
void EXTI3_IRQHandler(void) {
   if (EXTI->PR1 & EXTI_PR1_PIF3) {
      EXTI->PR1 = EXTI_PR1_PIF3;     // clear pending
      g_button_pressed_flag = 1;
      g_button_color_flag   = WHITE_CODE;
      g_button_event_ready  = 1;
   }
}

// EXTI line 4 -> PB4 (GREEN)
void EXTI4_IRQHandler(void) {
   if (EXTI->PR1 & EXTI_PR1_PIF4) {
      EXTI->PR1 = EXTI_PR1_PIF4;
      g_button_pressed_flag = 1;
      g_button_color_flag   = GREEN_CODE;
      g_button_event_ready  = 1;
   }
}

// EXTI lines 5..9 -> PB5 (YELLOW) is on this
void EXTI9_5_IRQHandler(void) {
   if (EXTI->PR1 & EXTI_PR1_PIF5) {
      EXTI->PR1 = EXTI_PR1_PIF5;
      g_button_pressed_flag = 1;
      g_button_color_flag   = YELLOW_CODE;
      g_button_event_ready  = 1;
   }
   // if you ever add more on lines 6..9, handle them here too
}

// EXTI lines 10..15 -> PB12 (BLUE), PB13 (RED)
void EXTI15_10_IRQHandler(void) {
   if (EXTI->PR1 & EXTI_PR1_PIF12) {
      EXTI->PR1 = EXTI_PR1_PIF12;
      g_button_pressed_flag = 1;
      g_button_color_flag   = BLUE_CODE;
      g_button_event_ready  = 1;
   }
   if (EXTI->PR1 & EXTI_PR1_PIF13) {
      EXTI->PR1 = EXTI_PR1_PIF13;
      g_button_pressed_flag = 1;
      g_button_color_flag   = RED_CODE;
      g_button_event_ready  = 1;
   }
}

/*
 * Function 5..8: Interrupt service routines
 * --------------------
 * each ISR:
 *  clears EXTI pending bit, sets g_button_pressed_flag = 1,
 *  sets g_button_color_flag = 1..5
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
// returns 1 on success, 0 on timeout
int read_user_color_until(uint32_t deadline_ms, uint32_t *color_out)
{
   while (1) {
      uint32_t now = get_ms();
      if ((int32_t)(deadline_ms - now) <= 0) {
         return 0;  // timeout
      }

      if (g_button_event_ready) {
    	  __disable_irq();
         g_button_event_ready = 0;
         __enable_irq();
         *color_out = g_button_color_flag;  // 1..5
         return 1;                          // success
      }
   }
}

/*
 * Function 9: level_up
 * --------------------
 * checks if button_pressed_flag was toggled
 *
 *	takes in: nothing
 *
 *  returns: 1 = button was pressed
 *     0 = no button was pressed
 */
int level_up(void){
   while (!g_button_pressed_flag) {
      software_delay(1000);
   }

   g_button_pressed_flag = 0;
   return 1;
}

