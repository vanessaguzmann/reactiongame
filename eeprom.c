
/**
 * @file EEPROM.c
 * @brief I2C driver for 24LC256 on STM32L4A6
 *
 *  - I2C1 initialization for 400 kHz with timing: 0x00303D5B
 *  - Single-byte write: [Dev+W][AddrHi][AddrLo][Data] with AUTOEND
 *  - Single-byte read:  dummy write of 2 byte addr, repeated START, and 1 byte read
 *
 * @date Nov. 7, 2025
 * @author Karina Wilson
 */

#include "EEPROM.h"

#define EEPROM_ADDR7 0x51        //A2:A1:A0 = 0b001 -> 0x51.
#define I2C_TIMINGR  0x00303D5B  // 16 MHz I2C kernel

/* poll I2C1->ISR flags (blocking flag waits) */
/* avoids writing RX/TX registers too early */

// ready to write next byte
static inline void wait_TXIS(void) {
   while (!(I2C1->ISR & I2C_ISR_TXIS)) {
   }
}

// found a byte arrived to read
static inline void wait_RXNE(void) {
   while (!(I2C1->ISR & I2C_ISR_RXNE)) {
   }
}

// transfer of NBYTES done (no STOP)
static inline void wait_TC(void) {
   while (!(I2C1->ISR & I2C_ISR_TC)) {
   }
}

// STOP (clear reg)
static inline void wait_STOP(void) {
   while (!(I2C1->ISR & I2C_ISR_STOPF)) {
   }
   I2C1->ICR = I2C_ICR_STOPCF;
}

// GPIO PB8/PB9 AF4 open-drain pull-up
void I2C1_GPIO_Init(void) {
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

   // PB8, PB9 -> AF4
   GPIOB->AFR[1] &= ~((0xF << (0 * 4)) | (0xF << (1 * 4)));
   GPIOB->AFR[1] |= ((4 << (0 * 4)) | (4 << (1 * 4)));

   // Open-drain, used external 2k, medium/high speed
   GPIOB->OTYPER |= (1u << 8) | (1u << 9);
   GPIOB->PUPDR &= ~((3u << (8 * 2)) | (3u << (9 * 2)));      // no pull
   GPIOB->OSPEEDR &= ~((3u << (8 * 2)) | (3u << (9 * 2)));
   GPIOB->OSPEEDR |= ((2u << (8 * 2)) | (2u << (9 * 2)));      // medium/high

   GPIOB->MODER &= ~((3u << (8 * 2)) | (3u << (9 * 2)));
   GPIOB->MODER |= ((2u << (8 * 2)) | (2u << (9 * 2)));      // AF mode
}

void EEPROM_init(void) {
   I2C1_GPIO_Init();

   // Turn on HSI16 and select as I2C1 kernel clock
   RCC->CR |= RCC_CR_HSION;
   while ((RCC->CR & RCC_CR_HSIRDY) == 0) {
   }

   // I2C1SEL[13:12]: 00=PCLK1, 01=SYSCLK, 10=HSI16
   RCC->CCIPR = (RCC->CCIPR & ~(3u << 12)) | (2u << 12);

   RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

   // Clean reset
   RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST;
   RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST;

   I2C1->CR1 &= ~I2C_CR1_PE;        // disable I2C before config.
   I2C1->CR1 &= ~I2C_CR1_ANFOFF;    // analog filter ON
   I2C1->CR1 &= ~I2C_CR1_DNF;       // digital filter OFF

   I2C1->TIMINGR = I2C_TIMINGR;     // 0x00303D5B (matches HSI16)

   I2C1->CR2 &= ~I2C_CR2_ADD10;     // 7-bit

   // Clear all previous flags
   I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF | I2C_ICR_BERRCF | I2C_ICR_ARLOCF
         | I2C_ICR_OVRCF | I2C_ICR_PECCF;

   // enable I2C peripheral
   I2C1->CR1 |= I2C_CR1_PE;
}

void EEPROM_write(uint16_t addr, uint8_t data) {
   while (I2C1->ISR & I2C_ISR_BUSY) {     // wait until I2C bus is idle
   }
   I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF; // clear STOP and NACK flags

   // configure 3-byte write (AddrHi, AddrLo, Data) with auto STOP
   I2C1->CR2 = ((EEPROM_ADDR7 << 1) & I2C_CR2_SADD) | (3u << I2C_CR2_NBYTES_Pos)
         | I2C_CR2_AUTOEND;
   I2C1->CR2 &= ~I2C_CR2_RD_WRN;            // write
   I2C1->CR2 |= I2C_CR2_START;              // generate start condition

   wait_TXIS();
   I2C1->TXDR = (uint8_t) (addr >> 8); // send address high byte
   wait_TXIS();
   I2C1->TXDR = (uint8_t) (addr & 0xFF); // send address low byte
   wait_TXIS();
   I2C1->TXDR = data; // send data byte

   wait_STOP();                    // wait for STOP indicating transfer complete
}

uint8_t EEPROM_read(uint16_t addr) {
   while (I2C1->ISR & I2C_ISR_BUSY) {           // wait until I2C bus is idle
   }
   I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF; // clear STOP and NACK flags

   // 1st part: write 2 byte address (no AUTOEND, no STOP)
   I2C1->CR2 = ((0x51 << 1) & I2C_CR2_SADD) | (2u << I2C_CR2_NBYTES_Pos); // 2 bytes to send
   I2C1->CR2 &= ~I2C_CR2_RD_WRN; // write mode
   I2C1->CR2 |= I2C_CR2_START; // generate START condition

   while (!(I2C1->ISR & I2C_ISR_TXIS)) {  // wait TX ready
   };
   I2C1->TXDR = addr >> 8; // send address high byte

   while (!(I2C1->ISR & I2C_ISR_TXIS)) {
   };

   I2C1->TXDR = addr & 0xFF;  // send address low byte
   while (!(I2C1->ISR & I2C_ISR_TC)) {
   }                          // repeated START next

   // 2nd part: read 1 byte with AUTOEND (resulting in NACK+STOP)
   I2C1->CR2 = ((0x51 << 1) & I2C_CR2_SADD) | (1u << I2C_CR2_NBYTES_Pos)
         | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND; // 1 byte read
   I2C1->CR2 |= I2C_CR2_START;               // repeat start

   while (!(I2C1->ISR & I2C_ISR_RXNE)) {    // wait until data received
   }
   uint8_t b = I2C1->RXDR;                  // read data byte
   while (!(I2C1->ISR & I2C_ISR_STOPF)) {   // wait stop condition
   }
   I2C1->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF; // clear flags
   return b;                                    // return read byte
}

//loops thru characters in name, saves to 3 consecutive addresses
//last 2 addresses are for score (high byte and low byte)
void saveLeaderboard(Player *board, uint8_t count) {
    uint16_t addr = 0;
    for (uint8_t i = 0; i < count; i++) {
        for (uint8_t j = 0; j < NAME_LEN; j++)
            EEPROM_write(addr++, board[i].name[j]);
        EEPROM_write(addr++, (board[i].score >> 8) & 0xFF);
        EEPROM_write(addr++, board[i].score & 0xFF);
    }
}

//uses *board to point to each player in array
//reads initials, then score
uint8_t loadLeaderboard(Player *board) {
    uint16_t addr = 0;
    uint8_t count = 0;
    for (uint8_t i = 0; i < MAX_PLAYERS; i++) {
        for (uint8_t j = 0; j < NAME_LEN; j++)
            board[i].name[j] = EEPROM_read(addr++);
        	delay_ms(5);
        board[i].score = ((uint16_t)EEPROM_read(addr++) << 8);
        board[i].score |= EEPROM_read(addr++);
        if (board[i].score > 0 && board[i].score < 9999) //ensure score is valid
            count++;
    }
    return count;
}

void sortLeaderboard(Player *board, uint8_t count) {
	for(uint8_t i = 0; i < count-1; i++) {
		for (uint8_t j = 1; j < count; j++){
			if (board[i].score < board[j].score){
				Player temp = board[i]; //create temp variable so we don't lose entry
				board[j] = board[i];  //swap
				temp = board[j];

			}
		}

	}
}

uint8_t addScore(Player *board, uint8_t count, const char *name, uint16_t score) {
    if (count < MAX_PLAYERS) {
        for (uint8_t i = 0; i < NAME_LEN; i++)  //when !10 entries
            board[count].name[i] = name[i];
        board[count].score = score;
        count++; //increase count until reaching 10 players
    } else if (score > board[count - 1].score) { //if newest score > lowest score
        for (uint8_t i = 0; i < NAME_LEN; i++)
            board[count - 1].name[i] = name[i]; //replace lowest score with new score
        board[count - 1].score = score;
    }
    sortLeaderboard(board, count);
    saveLeaderboard(board, count);
    return count;
}
