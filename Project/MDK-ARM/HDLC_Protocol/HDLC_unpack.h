#ifndef _HDLC_UNPACK_
#define	_HDLC_UNPACK_
#include <stdlib.h>
#include <stdint.h>

uint8_t HDLC_UnpackWaitConfigParam(uint8_t* data, uint16_t len);
uint8_t HDLC_UnpackWaitAuthorization(uint8_t* data, uint16_t len);
uint8_t HDLC_UnpackWaitComand(float* Volt, uint8_t* data, uint16_t len);
#endif
