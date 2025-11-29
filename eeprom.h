
/**
 * @file EEPROM.h
 * @brief Header I2C driver for 24LC256 on STM32L4A6
 *
 *  - I2C1 initialization for 400 kHz with timing: 0x00303D5B
 *  - Single-byte write: [Dev+W][AddrHi][AddrLo][Data] with AUTOEND
 *  - Single-byte read:  dummy write of 2 byte addr, repeated START, and 1 byte read
 *
 * @date Nov. 7, 2025
 * @author William Chung + Vanessa Guzman
 */

#ifndef SRC_EEPROM_H_
#define SRC_EEPROM_H_

#include "stm32l4xx.h"
#include <stdint.h>

#define MAX_PLAYERS 10
#define NAME_LEN 3
#define EEPROM_START_ADDR 0x0000

typedef struct {
    char name[NAME_LEN];
    uint16_t score;
} Player;

/**
 * @brief configure PB8 (SCL) and PB9 (SDA) for I2C1 AF4
 */
void I2C1_GPIO_Init(void);

/**
 * @brief initialize I2C1 peripheral for EEPROM
 *        sets 7-bit address mode and TIMINGR = 0x00303D5B (400 kHz @ 16 MHz)
 */
void EEPROM_init(void);

/**
 * @brief write a single byte to EEPROM at given 16-bit addr
 *
 * @param addr  16-bit memory addr (0x0000–0x7FFF)
 * @param data  data byte to write
 */
void EEPROM_write(uint16_t addr, uint8_t data);

/**
 * @brief read a single byte from the EEPROM at  given 16-bit addr
 *
 * @param  16-bit memory addr
 * @return uint8_t  8-bit data read from EEPROM
 */
uint8_t EEPROM_read(uint16_t addr);
void saveLeaderboard(Player *board, uint8_t count);
uint8_t loadLeaderboard(Player *board);
void sortLeaderboard(Player *board, uint8_t count);
uint8_t addScore(Player *board, uint8_t count, const char *name, uint16_t score);
extern Player leaderboard[MAX_PLAYERS];

#endif /* SRC_EEPROM_H_ */
