#ifndef _HDLC_PACK_
#define  _HDLC_PACK_
#include <stdlib.h>
#include <stdint.h>

uint8_t HDLC_PackSendConfigParam(void (*uartSendDataCB)(uint8_t *data, uint16_t len));
uint8_t HDLC_PackSendAuthorization(void (*uartSendDataCB)(uint8_t *data, uint16_t len), uint8_t N);
uint8_t HDLC_PackSendComand(uint8_t N,void (*uartSendDataCB)(uint8_t *data, uint16_t len), uint8_t PosParam);
void HDLC_PackAdr(uint16_t meter_adr, uint8_t client_adr);
#endif
