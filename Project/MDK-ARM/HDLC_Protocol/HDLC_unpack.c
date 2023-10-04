#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "HDLC_unpack.h"
//#include "HDLC_Protocol.h"
#include "MAC_Pack.h"
#include "CRC.h"
//--------------------------------------------------------------------------------
typedef struct
{
  uint8_t MAC_start [11];
  uint8_t res[3];
  uint8_t req_res;
  uint8_t res_2[3];
  uint8_t tupe;
  uint8_t vole;
}t_answer;
//--------------------------------------------------------------------------------
uint8_t response_connect[] = { 0x7E, 0xA0, 0x23, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x73, 0xB9, 0x33, 0x81, 0x80, 0x14, 0x05, 0x02,
  0x04, 0x00, 0x06, 0x02, 0x04, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
  0x00, 0x01, 0x72, 0xE3, 0x7E };

uint8_t response_authorize[] = { 0x7E, 0xA0, 0x3A, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x30, 0xD6, 0xB7, 0xE6, 0xE7, 0x00, 0x61, 0x29,
  0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xA2, 0x03, 0x02, 0x01, 0x00,
  0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00, 0xBE, 0x10, 0x04, 0x0E, 0x08, 0x00, 0x06, 0x5F, 0x1F,
  0x04, 0x00, 0x00, 0x10, 0x15, 0x04, 0x00, 0x00, 0x07, 0x62, 0xC5, 0x7E };

//--------------------------------------------------------------------------------
uint8_t HDLC_UnpackWaitConfigParam(uint8_t* data, uint16_t len)
{
  if( memcmp(data, response_connect, sizeof(response_connect)) == 0 )
  {
    return 0;
  }

  return 1;
}
//--------------------------------------------------------------------------------
uint8_t HDLC_UnpackWaitAuthorization(uint8_t* data, uint16_t len)
{
  if( memcmp(data, response_authorize, sizeof(response_authorize)) == 0 )
  {
    return 0;
  }

  return 1;
}
//--------------------------------------------------------------------------------
uint8_t HDLC_UnpackVoltComand(float* Volt, uint8_t* data, uint16_t len)
{
  float* V = Volt;
  //*V = 0;
  uint8_t rev [4] = {0};

  uint8_t count = 0, i = 0;
  while ( (count < 2) && (i < len) )
  {
    if(data[i] == '~')
    {
      count++;
    }
    i++;
  }
  rev[3]=data[i-7];
  rev[2]=data[i-6];
  rev[1]=data[i-5];
  rev[0]=data[i-4];

//  rev = (rev >> 24)|((rev << 8) & 0x00FF0000)|((rev >> 8) & 0x0000FF00)|(rev >> 24);

  *V = *( (float*) rev );
/*
  if(200 < *V || *V < 300)
    {
      return 1;
    }
*/
  return 0;
}
//--------------------------------------------------------------------------------
uint8_t HDLC_UnpackCurrentComand(float* Current, uint8_t* data, uint16_t len)
{
  float* I = Current;
  *I = 0;
  uint8_t rev [4] = {0};    
  
  uint8_t count = 0, i = 0;
  while ( (count < 2) && (i < len) )
  {
    if(data[i] == '~')
    {
      count++;
    }
    i++;
  }
  rev[3]=data[i-7];
  rev[2]=data[i-6];
  rev[1]=data[i-5];
  rev[0]=data[i-4];

//  rev = (rev >> 24)|((rev << 8) & 0x00FF0000)|((rev >> 8) & 0x0000FF00)|(rev >> 24);

  *I = *( (float*) rev );
/*
  if(200 < *I || *I < 300)
    {
      return 1;
    }
*/
  return 0;
}

uint8_t HDLC_UnpackComand(float* Param, int64_t* Energe, uint8_t* data, uint16_t len)
{
  float* Par = Param;
  int64_t* Eng = Energe;
  t_answer* pac = (t_answer*)data;
  uint8_t rev[8] = {0};
  if (pac->tupe == 0x17)
  {
    rev[3] = *(&pac->vole + 0);
    rev[2] = *(&pac->vole + 1);
    rev[1] = *(&pac->vole + 2);
    rev[0] = *(&pac->vole + 3);

    *Par = *( (float*) rev );
  }
  
  else if(pac->tupe == 0x14)
  {
    rev[7] = *(&pac->vole + 0);
    rev[6] = *(&pac->vole + 1);
    rev[5] = *(&pac->vole + 2);
    rev[4] = *(&pac->vole + 3);
    rev[3] = *(&pac->vole + 4);
    rev[2] = *(&pac->vole + 5);
    rev[1] = *(&pac->vole + 6);
    rev[0] = *(&pac->vole + 7);
    
    *Eng = *( (int64_t*) rev );
  }
  return 0;
}