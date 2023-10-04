#ifndef _HDLC_UNPACK_
#define  _HDLC_UNPACK_
#include <stdlib.h>
#include <stdint.h>

uint8_t HDLC_UnpackWaitConfigParam(uint8_t* data, uint16_t len);
uint8_t HDLC_UnpackWaitAuthorization(uint8_t* data, uint16_t len);
uint8_t HDLC_UnpackVoltComand(float* Volt, uint8_t* data, uint16_t len);
uint8_t HDLC_UnpackCurrentComand(float* Current, uint8_t* data, uint16_t len);
//uint8_t HDLC_UnpackComand(float* Param, uint8_t* data, uint16_t len);
uint8_t HDLC_UnpackComand(float* Param, int64_t* Energe, uint8_t* data, uint16_t len);
#endif
