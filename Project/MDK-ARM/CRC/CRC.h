#ifndef _CRC_
#define _CRC_
#include <stdint.h>

uint16_t f_crc16(uint8_t* data, uint16_t len);
uint16_t swap(uint8_t* buf, uint16_t len);  // TODO swap - менять, swipe - махать

//#pragma pack(pop)
#endif//_CRC_
