#ifndef _HDLC_PACK_
#define  _HDLC_PACK_
#include <stdlib.h>
#include <stdint.h>

uint8_t HDLC_PackSendConfigParam(void (*uartSendDataCB)(uint8_t *data, uint16_t len));
uint8_t HDLC_PackSendAuthorization(void (*uartSendDataCB)(uint8_t *data, uint16_t len));
uint8_t HDLC_PackSendComand(uint8_t N,void (*uartSendDataCB)(uint8_t *data, uint16_t len));
#endif
