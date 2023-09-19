#include <stdint.h>
#include <string.h>
#include "Protocol.h"

#define POLLING_TIME    1000u
#define SEND_BUF_SIZE   100u

typedef enum
{
  NONE = 0,
  SEND_AUTHORIZE,
  WAIT_AUTHORIZE_ANSWER
} t_protocol_status;

static t_protocol_status Status;

static uint32_t LocalTime;
static t_InitParams Parameters;

uint8_t authorize_msg[] = {0,0,0,0,0,0};


uint8_t SendBuf[SEND_BUF_SIZE];

// ----------------------------------------------------------------------------
void ProtocolMain(void)
{
  uint32_t time = Parameters.getTicksCB();

  if( (time - LocalTime) < POLLING_TIME )
    return;
  LocalTime = Parameters.getTicksCB();

  // что-то делаем
  // Parameters.uartSendDataCB()
  switch(Status)
  {
    case NONE:
      Status = SEND_AUTHORIZE;
      break;
    case SEND_AUTHORIZE:
      // отправка сообщения авторизации
      
    default:
      break;
  }
}
// ----------------------------------------------------------------------------
void ProtocolInitParamsStructureReset(t_InitParams *init)
{
  memset(init, 0, sizeof(t_InitParams) );
}
// ----------------------------------------------------------------------------
void ProtocolInit(t_InitParams *init)
{
  Status = NONE;
  ProtocolInitParamsStructureReset(&Parameters);
  memcpy(&Parameters, init, sizeof(t_InitParams));
  LocalTime = Parameters.getTicksCB();
  memset(SendBuf, 0, SEND_BUF_SIZE);
}
// ----------------------------------------------------------------------------
void DataReceive(uint8_t* data, uint16_t len)
{

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
