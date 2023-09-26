#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "HDLC_Protocol.h"
#include "HDLC_unpack.h"
#include "HDLC_pack.h"

#define POLLING_TIME            1000u
#define SEND_BUF_SIZE            100u
#define GET_BUF_SIZE            100u
#define SEND_AUTHORIZATION_SIZE  100u
#define FLAG                    0x7E

typedef enum
{
  NONE = 0,
  FIRST_AUTHORIZATION,
  SEND_CONFIG_PARAM,
  WAIT_CONFIG_PARAM_ANSWER,
  SEND_AUTHORIZATION,
  WAIT_AUTHORIZATION_ANSWER,
  SEND_COMAND,
  WAIT_COMAND_ANSWER
} t_protocol_status;

typedef enum 
{
  SEND_1 = 0,
  GET_1,
  SEND_2,
  GET_2,
  SEND_3,
  GET_3,
  SEND_4,
  GET_4,
  SEND_5,
  GET_5,
  AUTHORIZATION_COMPLETED
} t_HDLC_Autorization_status;

typedef struct
{
  uint8_t *point;
  uint16_t size;
} t_PointAndSize;

bool GetCorrect = false;
float Volt = 1;
uint32_t LocalTime;

t_protocol_status Status;
t_HDLC_Autorization_status Autorization_status;
t_InitParams Parameters;
//--------------------------------------------------------------------------------

uint8_t send_1[] = { 0x7E, 0xA0, 0x23, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x93, 0x77, 0x28, 0x81, 0x80, 0x14, 0x05, 0x02,
0x04, 0x00, 0x06, 0x02, 0x04, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
0x00, 0x01, 0x72, 0xE3, 0x7E };

uint8_t get_1[] = { 0x7E, 0xA0, 0x23, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x73, 0xB9, 0x33, 0x81, 0x80, 0x14, 0x05, 0x02,
0x04, 0x00, 0x06, 0x02, 0x04, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
0x00, 0x01, 0x72, 0xE3, 0x7E };

uint8_t send_2[] = { 0x7E, 0xA0, 0x47, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x10, 0x17, 0x55, 0xE6, 0xE6, 0x00, 0x60, 0x36,
0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0x8A, 0x02, 0x07, 0x80, 0x8B,
0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x02, 0x01, 0xAC, 0x0A, 0x80, 0x08, 0x31, 0x32, 0x33, 0x34,
0x35, 0x36, 0x37, 0x38, 0xBE, 0x10, 0x04, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 0x1F, 0x04,
0x00, 0xFF, 0xFF, 0xFF, 0x08, 0x00, 0xD5, 0x24, 0x7E };

uint8_t get_2[] = { 0x7E, 0xA0, 0x3A, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x30, 0xD6, 0xB7, 0xE6, 0xE7, 0x00, 0x61, 0x29,
0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xA2, 0x03, 0x02, 0x01, 0x00,
0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00, 0xBE, 0x10, 0x04, 0x0E, 0x08, 0x00, 0x06, 0x5F, 0x1F,
0x04, 0x00, 0x00, 0x10, 0x15, 0x04, 0x00, 0x00, 0x07, 0x62, 0xC5, 0x7E };

uint8_t send_3[] = { 0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x32, 0x4A, 0x53, 0xE6, 0xE6, 0x00, 0xC0, 0x01,
0x40, 0x00, 0x01, 0x00, 0x00, 0x60, 0x01, 0x08, 0xFF, 0x02, 0x00, 0xDB, 0x82, 0x7E };

uint8_t get_3[] = { 0x7E, 0xA0, 0x35, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x52, 0x36, 0xEE, 0xE6, 0xE7, 0x00, 0xC4, 0x01,
0x40, 0x00, 0x09, 0x20, 0x39, 0x2E, 0x38, 0x2E, 0x33, 0x30, 0x37, 0x36, 0x2E, 0x31, 0x2E, 0x39,
0x2E, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x24, 0x06, 0x7E };

uint8_t send_4[] = { 0x7E, 0xA0, 0x1C, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x54, 0x7A, 0x55, 0xE6, 0xE6, 0x00, 0xC0, 0x01,
0x40, 0x00, 0x01, 0x00, 0x00, 0x60, 0x01, 0x00, 0xFF, 0x02, 0x00, 0x03, 0x67, 0x7E };

uint8_t get_4[] = { 0x7E, 0xA0, 0x35, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x74, 0x02, 0xAA, 0xE6, 0xE7, 0x00, 0xC4, 0x01,
0x40, 0x00, 0x09, 0x20, 0x30, 0x31, 0x32, 0x36, 0x33, 0x36, 0x31, 0x38, 0x37, 0x38, 0x30, 0x34,
0x34, 0x35, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x1D, 0xCA, 0x7E };

uint8_t send_5[] = { 0x7E, 0xA0, 0x0A, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x53, 0xC1, 0xCC, 0x7E };

uint8_t get_5[] = { 0x7E, 0xA0, 0x21, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x73, 0x02, 0x04, 0x81, 0x80, 0x12, 0x05, 0x01,
0x80, 0x06, 0x01, 0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01, 0x53, 0x3B, 0x7E };
//--------------------------------------------------------------------------------

t_PointAndSize ConnectAutorizationSendPacks[] = { send_1, sizeof(send_1),
                                                  send_2, sizeof(send_2),
                                                  send_3, sizeof(send_3),
                                                  send_4, sizeof(send_4),
                                                  send_5, sizeof(send_5), };

t_PointAndSize ConnectAutorizationGetPacks[] = {  get_1, sizeof(get_1),
                                                  get_2, sizeof(get_2),
                                                  get_3, sizeof(get_3),
                                                  get_4, sizeof(get_4),
                                                  get_5, sizeof(get_5), };
//--------------------------------------------------------------------------------

uint8_t authorize_msg[] = {0,0,0,0,0,0};

uint8_t HDLC_SendBuf[SEND_BUF_SIZE];
uint8_t HDLC_GetBuf[GET_BUF_SIZE];

uint16_t GetBufHead;
bool RecordingInProgress;

void HDLC_ConnectAutorization(void);

void TimeOutReset(void);
bool IsTimeOut(void);

bool HDLC_SendData(uint8_t *data, uint16_t len); 
uint32_t GetTicks(void);

bool HDLC_SendData(uint8_t *data, uint16_t len)
{
  if(Parameters.uartSendDataCB == NULL)
    return false;

// Parameters.uartSendDataCB()
// сброс таймаута
}

uint32_t GetTicks(void)
{
  if(Parameters.getTicksCB == NULL)
    return 0;
  return Parameters.getTicksCB();
}
// ----------------------------------------------------------------------------
void HDLC_ProtocolMain(void) 
{
  uint32_t time = Parameters.getTicksCB();

  if( (time - LocalTime) < POLLING_TIME )
    return;
  LocalTime = Parameters.getTicksCB();
  //memcmp
  // что-то делаем
  // Parameters.uartSendDataCB()

  // проверка таймаута if(IsTimeOut()) Status = NONE
  // TODO проверяем флаг принятых данных и отправляем на проверку дяльше

  switch(Status)
  {
    case NONE:
      Autorization_status = SEND_1;
      Status = FIRST_AUTHORIZATION;
      break;
    case FIRST_AUTHORIZATION:
      HDLC_ConnectAutorization(); // (HDLC_GetBuf, len_data)
      if(Autorization_status == AUTHORIZATION_COMPLETED)
      {
        Status = SEND_CONFIG_PARAM;
      }
      break;
    case SEND_CONFIG_PARAM:
      HDLC_PackSendConfigParam(Parameters.uartSendDataCB);
      Status = WAIT_CONFIG_PARAM_ANSWER;
      break;
    case WAIT_CONFIG_PARAM_ANSWER:
      if(HDLC_UnpackWaitConfigParam(HDLC_GetBuf, GET_BUF_SIZE) == 0)
      {
        Status = SEND_AUTHORIZATION;
        GetBufHead = 0;
      }
      break;
    case SEND_AUTHORIZATION:
      HDLC_PackSendAuthorization(Parameters.uartSendDataCB);
      Status = WAIT_AUTHORIZATION_ANSWER;
      break;
    case WAIT_AUTHORIZATION_ANSWER:
      if(HDLC_UnpackWaitAuthorization(HDLC_GetBuf, GET_BUF_SIZE) == 0)
      {
        Status = SEND_COMAND;
        GetBufHead = 0;
      }
      break;
    case SEND_COMAND:
      HDLC_PackSendComand(Parameters.uartSendDataCB);
      Status =  WAIT_COMAND_ANSWER;  
      break;
    case WAIT_COMAND_ANSWER:
      HDLC_UnpackWaitComand(&Volt, HDLC_GetBuf, GET_BUF_SIZE);
      break;
    default:
      break;
  }

  // если данные отправляются и принимаются нормально, то сброс таймаута
}
// ----------------------------------------------------------------------------
void HDLC_ProtocolInitParamsStructureReset(t_InitParams *init)
{
  memset(init, 0, sizeof(t_InitParams) );
  GetBufHead = 0;
}
// ----------------------------------------------------------------------------
void HDLC_ProtocolInit(t_InitParams *init)
{
  Status = NONE;
  HDLC_ProtocolInitParamsStructureReset(&Parameters);
  memcpy(&Parameters, init, sizeof(t_InitParams));
  LocalTime = Parameters.getTicksCB();
  memset(HDLC_SendBuf, 0, SEND_BUF_SIZE);
  Autorization_status = SEND_1;
//  void TimeOutReset(void);
}
// ----------------------------------------------------------------------------
void HDLC_ProtocolPackAuthorization(uint8_t pasword)
{
  memcmp(HDLC_SendBuf, authorize_msg, sizeof(authorize_msg));
}
// ----------------------------------------------------------------------------
void HDLC_ProtocolDataReceive(uint8_t* data, uint16_t len)
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
      HDLC_GetBuf[ GetBufHead ] = data[i];
      GetBufHead++;
      RecordingInProgress = false;
    }

    if(RecordingInProgress == true)
    {
      HDLC_GetBuf[ GetBufHead ] = data[i];
      GetBufHead++;
    }
  }
}
// ----------------------------------------------------------------------------
//Показания_____________________
float GetVoltageA(void)
{
  return 0;
}
// ----------------------------------------------------------------------------
float GetVoltageB(void)
{
  return 0;
}
float GetVoltageC(void)
{
  return 0;
}
float GetCurrentA(void)
{
  return 0;
}
float GetCurrentB(void)
{
  return 0;
}
float GetCurrentC(void)
{
  return 0;
}
float GetPowerA(void)
{
  return 0;
}
float GetPowerB(void)
{
  return 0;
}
float GetPowerC(void)
{
  return 0;
}
float GetEnergyActiveImport(void)
{
  return 0;
}
float GetEnergyActiveExport(void)
{
  return 0;
}
float GetEnergyReactiveImport(void)
{
  return 0;
}
float GetEnergyReactiveExport(void)
{
  return 0;
}
//--------------------------------------------------------------------------------
void HDLC_SendConnectAutorization(uint8_t num)
{
  memcpy(HDLC_SendBuf, ConnectAutorizationSendPacks[num].point, ConnectAutorizationSendPacks[num].size);
  Parameters.uartSendDataCB(HDLC_SendBuf, ConnectAutorizationSendPacks[num].size);
}
//--------------------------------------------------------------------------------
bool HDLC_GetConnectAutorization(uint8_t num)
{
  if(memcmp(ConnectAutorizationGetPacks[num].point, HDLC_GetBuf, ConnectAutorizationGetPacks[num].size) == 0)
  {
    return true;
  }
  return false;

//  return (memcmp(ConnectAutorizationGetPacks[num].point, HDLC_GetBuf, ConnectAutorizationGetPacks[num].size) == 0);
}
//--------------------------------------------------------------------------------
void HDLC_ConnectAutorization(void) // (HDLC_GetBuf, len_data) return 0 - нет данных, 1 - данные нашлись, 2 - в процессе, 3 - окончание авторизации
{
//  int result = 0;
//  bool data_find = false;

  switch(Autorization_status)
  {
    case SEND_1:
      HDLC_SendConnectAutorization(0); // (void)
      Autorization_status = GET_1;
      break;
      // отправка сообщения авторизации
    case GET_1:
      if( /* data_find = */HDLC_GetConnectAutorization(0)/*(HDLC_GetBuf, len_data) return bool */ )
      {
        Autorization_status = SEND_2;
        GetBufHead = 0;
      }
    break;
    case SEND_2:
      HDLC_SendConnectAutorization(1);
      Autorization_status = GET_2;
      break;
      // отправка сообщения авторизации
    case GET_2:
      if(  HDLC_GetConnectAutorization(1) )
      {
        Autorization_status = SEND_3;
        GetBufHead = 0;
      }
      break;
    case SEND_3:
      HDLC_SendConnectAutorization(2);
      Autorization_status = GET_3;
      break;
      // отправка сообщения авторизации
    case GET_3:
      if( HDLC_GetConnectAutorization(2) )
      {
        Autorization_status = SEND_4;
        GetBufHead = 0;
      }
      break;
    case SEND_4:
      HDLC_SendConnectAutorization(3);
      Autorization_status = GET_4;
      break;
      // отправка сообщения авторизации
    case GET_4:
      if( HDLC_GetConnectAutorization(3) )
      {
        Autorization_status = SEND_5;
        GetBufHead = 0;
      }
      break;
    case SEND_5:
      HDLC_SendConnectAutorization(4);
      Autorization_status = GET_5;
      break;
      // отправка сообщения авторизации
    case GET_5:
      if(HDLC_GetConnectAutorization(4))
      {
        GetCorrect = true;
        Autorization_status = AUTHORIZATION_COMPLETED;
        GetBufHead = 0;
      }
      break;
    default:
      break;
  }

//  return result;
}
