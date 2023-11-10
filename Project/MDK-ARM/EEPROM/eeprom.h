/**
  ******************************************************************************
  * @file           : eeprom.h
  * @brief          : Eeprom emulation driver
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef EEPROM_H
#define EEPROM_H



/* Includes ------------------------------------------------------------------*/

#include "stm32f1xx_hal.h"



/* Declarations and definitions ----------------------------------------------*/

#define PAGE_DATA_OFFSET                                                8
#define PAGE_DATA_SIZE                                                  8

#define PARAM_1                                                         0x00000001
#define PARAM_2                                                         0x00000002
#define PARAM_3                                                         0x00000003
#define PARAM_4                                                         0x00000004
#define PARAM_5                                                         0x00000005
#define PARAM_6                                                         0x00000006
#define PARAM_7                                                         0x00000007
#define PARAM_8                                                         0x00000008
#define PARAM_9                                                         0x00000009
#define PARAM_10                                                        0x00000010
#define VAR_NUM                                                         10

#define PAGE_0_ADDRESS                                                  0x0801F800
#define PAGE_1_ADDRESS                                                  0x0801FC00
#define PAGE_SIZE                                                       1024



typedef enum {
  PAGE_CLEARED = 0xFFFFFFFF,
  PAGE_ACTIVE = 0x00000000,
  PAGE_RECEIVING_DATA = 0x55555555,
} PageState;

typedef enum {
  PAGE_0 = 0,
  PAGE_1 = 1,
  PAGES_NUM = 2,
} PageIdx;

typedef enum {
  EEPROM_OK = 0,
  EEPROM_ERROR = 1,
} EepromResult;



/* Functions -----------------------------------------------------------------*/

extern EepromResult EEPROM_Init(void);
extern EepromResult EEPROM_Read(uint32_t varId, uint32_t *varValue);
extern EepromResult EEPROM_Write(uint32_t varId, uint32_t varValue);
uint32_t PARAM(uint8_t num);

#endif // #ifndef EEPROM_H
