/*
------------------------------------------------------------------------------
led_timer.c
------------------------------------------------------------------------------
* USER CODE BEGIN Header
******************************************************************************
* EE 329 led_timer.c
******************************************************************************
* @file           : led_timer.c
* @brief          : led_timer program body
* project         : EE 329 Final Project
* authors         : Vanessa G
* version         : 1
* date            : 11/21/2025
* compiler        : STM32CubeIDE v.1.19.0
* target          : NUCLEO-L4A6ZG
* clocks          : 4 MHz MSI to AHB2
* wiring    	  : PC8-12 connected to 560ohm resistors, LEDS lead to GND
* attachment      : LED1-5 attached to PC8-12, respectively
* 			    	1: white
* 					2: yellow
* 					3: green
* 					4: blue
* 					5: red
* @attention  	  : (c) 2023 STMicroelectronics.  All rights reserved.
******************************************************************************
* REVISION HISTORY
* 10/08/25	Created file
* 11/19/25	Removed nibble count from V1
* 			Included delay and moved software delay
* 11/23/25    Added array typedef structure and leveling logic
******************************************************************************
*/

#include "lcd.h"
#include "led_timer.h"
#include "delay.h"
#include "nvic.h"
#include "main.h"

volatile uint32_t sw_delay_ms = 3000;

/*
 * Function 1:  led_init
 * --------------------
 * enables GPIOC clock, sets PC8..PC12 as output, push-pull, no pull, high speed
 *    and ensures all LEDS are off
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
void led_init(void) {
	// Enable GPIOC clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	// PC8..PC12 as output, push-pull, no pull, high speed
	GPIOC->MODER   &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9 |
	                    GPIO_MODER_MODE10 | GPIO_MODER_MODE11 |
						GPIO_MODER_MODE12);
	GPIOC->MODER   |=  (GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0 |
	                    GPIO_MODER_MODE10_0 | GPIO_MODER_MODE11_0 |
	                    GPIO_MODER_MODE12_0);
	GPIOC->OTYPER  &= ~(GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9 |
	                    GPIO_OTYPER_OT10 | GPIO_OTYPER_OT11 |
						GPIO_OTYPER_OT12);
	GPIOC->PUPDR   &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9 |
	                    GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD11 |
						GPIO_PUPDR_PUPD12);
	GPIOC->OSPEEDR |=  (3u << GPIO_OSPEEDR_OSPEED8_Pos) |
	                   (3u << GPIO_OSPEEDR_OSPEED9_Pos) |
	                   (3u << GPIO_OSPEEDR_OSPEED10_Pos)|
	                   (3u << GPIO_OSPEEDR_OSPEED11_Pos)|
					   (3u << GPIO_OSPEEDR_OSPEED12_Pos);

	// Ensure all LEDS are off
	GPIOC->BRR = LED_MASK;
}

/*
 * Function 2:  flash_rnd_led
 * --------------------
 * selects an LED code randomly
 *
 *	takes in: nothing
 *
 *  returns: the color code of what led was flashed
 */
uint32_t flash_rnd_led(void) {
   // Declare ranges with enums!
   enum { WHITE_START  = 0x00000000u, WHITE_END  = 0x33333333u };
   // White: 0 .. 858,993,459
   enum { YELLOW_START = 0x33333334u, YELLOW_END = 0x66666666u };
   // Yellow: 858,993,460 .. 1,717,986,918
   enum { GREEN_START  = 0x66666667u, GREEN_END  = 0x99999999u };
   // Green: 1,717,986,919 .. 2,576,980,377
   enum { BLUE_START   = 0x9999999Au, BLUE_END   = 0xCCCCCCCCu };
   // Blue: 2,576,980,378 .. 3,435,973,836
   enum { RED_START    = 0xCCCCCCCDu, RED_END    = 0xFFFFFFFFu };
   // Red: 3,435,973,837 .. 4,294,967,295

   // Make a random number generate
   uint32_t random_num = rng();

   // Create LED color code variable
   uint32_t led_color_code = 0;

   // Turn on LED if in LEDS range
   switch ( random_num ) {
      case WHITE_START ... WHITE_END:
		 led_color_code = 1;
	     break;
      case YELLOW_START ... YELLOW_END:
		 led_color_code = 2;
         break;
      case GREEN_START ... GREEN_END:
		 led_color_code = 3;
	     break;
      case BLUE_START ... BLUE_END:
		 led_color_code = 4;
	     break;
      case RED_START ... RED_END:
		 led_color_code = 5;
	     break;
   }
   return led_color_code;
}

/*
 * Function 2:  show_led
 * --------------------
 * actually flashes the led
 *
 *	takes in: uint32_t color code
 *
 *  returns: nothing
 */
void show_led(uint32_t color) {
   switch (color) {
      case 1:
	    // Turn on all LEDS
		GPIOC->BSRR = GPIO_PIN_8;
		// Call a visible delay
		software_delay(1500);
		// Ensure all LEDS are off
		GPIOC->BRR = LED_MASK;
		// Call a visible delay
		software_delay(1500);
		break;
      case 2:
		 // Turn on all LEDS
		 GPIOC->BSRR = GPIO_PIN_9;
		 // Call a visible delay
		 software_delay(1500);
		 // Ensure all LEDS are off
		 GPIOC->BRR = LED_MASK;
		 // Call a visible delay
		 software_delay(1500);
   		 break;
	  case 3:
		 // Turn on all LEDS
		 GPIOC->BSRR = GPIO_PIN_10;
		 // Call a visible delay
		 software_delay(1500);
		 // Ensure all LEDS are off
		 GPIOC->BRR = LED_MASK;
		 // Call a visible delay
		 software_delay(1500);
		 break;
	  case 4:
		 // Turn on all LEDS
		 GPIOC->BSRR = GPIO_PIN_11;
		 // Call a visible delay
		 software_delay(1500);
		 // Ensure all LEDS are off
		 GPIOC->BRR = LED_MASK;
		 // Call a visible delay
		 software_delay(1500);
		 break;
	  case 5:
		 // Turn on all LEDS
		 GPIOC->BSRR = GPIO_PIN_12;
		 // Call a visible delay
		 software_delay(1500);
		 // Ensure all LEDS are off
         GPIOC->BRR = LED_MASK;
		 // Call a visible delay
		 software_delay(1500);
		 break;
	}
}

/*
 * Function 3: sequence_init
 * --------------------
 * inspired by "Serialise a strut containing a flexible array"
 *    https://tinyurl.com/9vexrzem @ Arduino Stack Exchange
 *
 * initializes a variable of type sequence to zero length
 *
 *	takes in: variable address of type sequence
 *
 *  returns: nothing
 */
void Sequence_Init(Sequence *seq) {
    seq->length = 0;
}

/*
 * Function 4: sequence_Append
 * --------------------
 * inspired by "Serialise a strut containing a flexible array"
 *    https://tinyurl.com/9vexrzem @ Arduino Stack Exchange
 *
 * appends additional color code
 *
 *	takes in: variable address of type sequence, code value
 *
 *  returns: 1 = success
 *           0 = fail
 */
uint8_t Sequence_Append(Sequence *seq, uint8_t value) {
   if (seq->length >= MAX_SEQ_LEN) {
      return 0; // fail
   }
   seq->data[seq->length++] = value;
   return 1; // success
}

/*
 * Function 5: generate_led_sequence
 * --------------------
 * inspired by "Serialise a strut containing a flexible array"
 *    https://tinyurl.com/9vexrzem @ Arduino Stack Exchange
 *
 * generates a sequence of leds according to level length
 *
 *	takes in: variable address of type sequence
 *
 *  returns: nothing
 */
void generate_led_sequence(Sequence *seq) {
   // Append one new random LED index (0..3)
   uint8_t next = flash_rnd_led(); // your function
   Sequence_Append(seq, next);
}

/*
 * Function 6: show_sequence
 * --------------------
 * inspired by "Serialise a strut containing a flexible array"
 *    https://tinyurl.com/9vexrzem @ Arduino Stack Exchange
 *
 * flashes sequence of the leds
 *
 *	takes in: variable address of type sequence
 *
 *  returns: nothing
 */
void show_sequence(const Sequence *seq) {
   for (uint32_t i = 0; i < seq->length; i++) {
      show_led(seq->data[i]);   // turn on corresponding LED
//        software_delay(sw_delay_ms);           // visible time
      GPIOC->BRR = LED_MASK;
//        software_delay(sw_delay_ms);
   }
}

/*
 * Function 7: run_reaction_game
 * --------------------
 * runs reaction game starting with a sequence, level @ 1, and user seq
 *    hops into infinite while loop, generates the values for sequence,
 *    flashes sequence, awaits user input, and compares user seq with
 *    sequence
 *
 *    interrupt 1 = wrong, gave up
 *    interrupt 2 = passed!
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
void run_reaction_game(void) {
    Sequence seq;
    uint32_t level = 1;

    while (1) {
        if (level > MAX_SEQ_LEN) {
            // Player won
            // trigger_interrupt(2);
            return;
        }

        // 1) Generate a fresh sequence of length = level
        Sequence_Init(&seq);
        for (uint32_t lvl_idx = 0; lvl_idx < level; lvl_idx++) {
            uint8_t next = flash_rnd_led();   // returns color code only
            Sequence_Append(&seq, next);
        }

        // 2) Show the whole sequence
        show_sequence(&seq);
        software_delay(3000);

        // 3) Get user input
        const uint32_t ANSWER_WINDOW_MS = 30000U; // 30 seconds to answer
        uint32_t t_start  = get_ms();
        uint32_t deadline = t_start + ANSWER_WINDOW_MS;

        Sequence user_seq;
        Sequence_Init(&user_seq);

        // Collect exactly seq.length button presses (or timeout)
        for (uint32_t seq_idx = 0; seq_idx < seq.length; seq_idx++) {
            uint32_t input_color;

            // Wait for a button press until deadline
            if (!read_user_color_until(deadline, &input_color)) {
                // Timeout
                // trigger_interrupt(1);
                return;
            }

            Sequence_Append(&user_seq, (uint8_t)input_color);
        }

        // 4) Compare sequences
        bool correct = true;
        for (uint32_t seq_idx = 0; seq_idx < seq.length; seq_idx++) {
            if (user_seq.data[seq_idx] != seq.data[seq_idx]) {
                correct = false;
                break;
            }
        }

        if (!correct) {
            // Wrong answer
            // trigger_interrupt(1);
            return;
        }

        // If we get here, the whole sequence was correct
        // trigger_interrupt(2);
        level++;
    }
}

/*
 * Function 8: flash_led
 * --------------------
 * flash all leds
 *
 *	takes in: nothing
 *
 *  returns: nothing
 */
void flash_led(void) {
	// Turn on all LEDS
	GPIOC->BSRR = LED_MASK;

	// Call a visible delay
	software_delay(3000);

	// Ensure all LEDS are off
	GPIOC->BRR = LED_MASK;

	// Call a visible delay
	software_delay(3000);
}

