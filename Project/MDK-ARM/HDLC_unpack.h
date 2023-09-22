#ifndef _HDLC_UNPACK_
#define	_HDLC_UNPACK_
#include <stdlib.h>
#include <stdint.h>

uint8_t HDLC_UnpackWaitConfigParam(void);
uint8_t HDLC_UnpackWaitAuthorization(void);
uint8_t HDLC_UnpackWaitComand(void);

#endif
