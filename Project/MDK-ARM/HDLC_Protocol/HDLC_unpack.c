#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "HDLC_unpack.h"
#include "MAC_Pack.h"
#include "CRC.h"
//--------------------------------------------------------------------------------
#define FLOAT      0x17
#define INT_64     0x14
typedef struct
{
  uint8_t MAC_start [11];
  uint8_t res[3];
  uint8_t req_res;
  uint8_t res_2[3];
  uint8_t type;
  uint8_t value;
}t_answer;

typedef struct
{
  uint8_t MAC_start [11];
  uint8_t res[19];
  uint8_t result_connect;
}t_authorization_answer;
//--------------------------------------------------------------------------------
uint8_t response_connect[] = { 0x7E, 0xA0, 0x23, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x73, 0xB9, 0x33, 0x81, 0x80, 0x14, 0x05, 0x02,
  0x04, 0x00, 0x06, 0x02, 0x04, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
  0x00, 0x01, 0x72, 0xE3, 0x7E };

uint8_t response_authorize[] = { 0x7E, 0xA0, 0x3A, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x30, 0xD6, 0xB7, 0xE6, 0xE7, 0x00, 0x61, 0x29,
  0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xA2, 0x03, 0x02, 0x01, 0x00,
  0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00, 0xBE, 0x10, 0x04, 0x0E, 0x08, 0x00, 0x06, 0x5F, 0x1F,
  0x04, 0x00, 0x00, 0x10, 0x15, 0x04, 0x00, 0x00, 0x07, 0x62, 0xC5, 0x7E };
uint8_t DA_SA_response[] = {0x41, 0x00, 0x02, 0x44, 0xC9};
//--------------------------------------------------------------------------------
uint8_t HDLC_UnpackWaitConfigParam(uint8_t* data, uint16_t len)
{
  t_HDLC_packet_begin* pack_begin = (t_HDLC_packet_begin*) data;
  
  /*
  if( memcmp(data, response_connect, sizeof(response_connect)) == 0 )
  {
    return 0;
  }
  */
  if (memcmp(pack_begin->DA_SA, DA_SA_response, sizeof(DA_SA_response)) == 0)
  {
    return 0;
  }
  return 1;
}
//--------------------------------------------------------------------------------
uint8_t HDLC_UnpackWaitAuthorization(uint8_t* data, uint16_t len)
{
  /*
  if( memcmp(data, response_authorize, sizeof(response_authorize)) == 0 )
  {
    return 0;
  }
  */
  t_authorization_answer* pack = (t_authorization_answer*) data;
  if (pack->result_connect == 1)
    return 0;
  return 1;
}
//--------------------------------------------------------------------------------
// TODO задефайнить 0x17 и 0x14
uint8_t HDLC_UnpackComand(void* Param, uint8_t* data, uint16_t len)
{
  t_answer* pac = (t_answer*)data;
  uint8_t rev[8] = {0};
  if (pac->type == FLOAT)
  {
    rev[3] = *(&pac->value + 0);
    rev[2] = *(&pac->value + 1);
    rev[1] = *(&pac->value + 2);
    rev[0] = *(&pac->value + 3);

    float* Par = Param;
    *Par = *( (float*) rev );
  }
  
  else if(pac->type == INT_64)
  {
    rev[7] = *(&pac->value + 0);
    rev[6] = *(&pac->value + 1);
    rev[5] = *(&pac->value + 2);
    rev[4] = *(&pac->value + 3);
    rev[3] = *(&pac->value + 4);
    rev[2] = *(&pac->value + 5);
    rev[1] = *(&pac->value + 6);
    rev[0] = *(&pac->value + 7);
    
    int64_t* Par = Param;
    *Par = *( (int64_t*) rev );
  }
  return 0;
}

void HDLC_UnPackAdr(uint16_t meter_adr, uint8_t client_adr)
{
  if ( (meter_adr > 0x3FFF) || (client_adr > 0x7F) )
    return;
  DA_SA_response[3] = (meter_adr >> 7)<<1;
  DA_SA_response[4] = ( (meter_adr & 0x7f)<<1 )|1;
  
  DA_SA_response[0] = (client_adr << 1)|1;
}
