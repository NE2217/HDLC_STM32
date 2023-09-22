#ifndef _HDLC_PACK_
#define	_HDLC_PACK_
#include <stdlib.h>
#include <stdint.h>

uint8_t HDLC_PackSendConfigParam(void);
uint8_t HDLC_PackSendAuthorization(void);
uint8_t HDLC_PackSendComand(void);

#endif
