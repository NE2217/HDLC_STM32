#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "HDLC_pack.h"
#include "MAC_Pack.h"
/*
bool RecordingInProgress = false;
 
void HDLC_ProtocolDataReceive(uint8_t* data, uint16_t len, uint8_t Buf, uint16_t Buf_size)
{
//  for(int i = 0; i < len; ++i)
//  {
//    if(!RecordingInProgress)
//    {
//      if(data[i] == FLAG)
//      {
//        RecordingInProgress = true;
//      }
//    }

//    if(RecordingInProgress)
//    {
//      HDLC_GetBuf[ GetBufHead++ ] = data[i];

//      if(data[i] == FLAG)
//      {
//        RecordingInProgress = false;
////        GetBufHead = 0;
//      }
////      if(длина данных превысила буфер)
////      if(время ожидания истекло)
//    }
//  }

  for(int i = 0; i < len; ++i)
  {
    if( (data[i] == FLAG) && !RecordingInProgress)
    {
      RecordingInProgress = true;
    }
    else if( (data[i] == FLAG) && RecordingInProgress)
    {
      HDLC_GetBuf[ GetBufHead++ ] = data[i];
      RecordingInProgress = false;
    }

    if(RecordingInProgress == true)
    {
      HDLC_GetBuf[ GetBufHead ] = data[i];
    }
  }
}
  return 0;
}

uint8_t HDLC_PackSendAuthorization(void (*uartSendDataCB)(uint8_t *data, uint16_t len))
{  
  uartSendDataCB(request_authorize, sizeof(request_authorize));
  return 0;
}

uint8_t HDLC_PackSendComand(void (*uartSendDataCB)(uint8_t *data, uint16_t len))
{  
  uartSendDataCB(request_volt_C, sizeof(request_volt_C));
  return 0;
}
*/
