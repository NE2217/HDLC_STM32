#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "HDLC_Protocol.h"

#define POLLING_TIME						1000u
#define SEND_BUF_SIZE						100u
#define GET_BUF_SIZE						100u
#define SEND_AUTHORIZATION_SIZE	100u

bool HDLC_ready_to_read = true;
bool HDLC_ready_to_send = false;

typedef enum
{
  NONE = 0,
  SEND_AUTHORIZATION,
  WAIT_AUTHORIZATION_ANSWER,
  SEND_CONFIG_PARAM,
	WAIT_CONFIG_PARAM_ANSWER,
  SEND_COMAND,
	WAIT_COMAND_ANSWER
} t_protocol_status;

static t_protocol_status Status;

static uint32_t LocalTime;
static t_InitParams Parameters;

uint8_t authorize_msg[] = {0,0,0,0,0,0};

uint8_t HDLC_SendBuf[SEND_BUF_SIZE];
uint8_t HDLC_GetBuf[GET_BUF_SIZE];

// ----------------------------------------------------------------------------
void DefaultParams(void)
{
	Parameters.max_cadr_reception_data = 1024;
  Parameters.max_cadr_transmission_data = 1024;
  Parameters.max_window_reception_data = 1;
  Parameters.max_window_transmission_data = 1;
  Parameters.server_address = 148681;
  Parameters.client_address = 65;
	memcpy(Parameters.pasword, "12345678", 8);
}
// ----------------------------------------------------------------------------
void ProtocolMain(void)
{
  uint32_t time = Parameters.getTicksCB();

  if( (time - LocalTime) < POLLING_TIME )
    return;
  LocalTime = Parameters.getTicksCB();
	//memcmp
  // что-то делаем
  // Parameters.uartSendDataCB()
  switch(Status)
  {
    case NONE:
      Status = SEND_AUTHORIZATION;
      break;
    case SEND_AUTHORIZATION:
      // отправка сообщения авторизации
      
    default:
      break;
  }
}
void HDLC_ProtocolMain(uint8_t* sendBuf, uint16_t lenSendBuf, uint8_t* getBuf, uint16_t lenGetBuf)
{
  uint32_t time = Parameters.getTicksCB();

  if( (time - LocalTime) < POLLING_TIME )
    return;
  LocalTime = Parameters.getTicksCB();
	//memcmp
  // что-то делаем
  // Parameters.uartSendDataCB()
  switch(Status)
  {
    case NONE:

			Status = SEND_AUTHORIZATION;
      break;
    case SEND_AUTHORIZATION:
			memcpy(HDLC_SendBuf, "SEND_AUTHORIZATION", sizeof("SEND_AUTHORIZATION"));
			memcpy(sendBuf, HDLC_SendBuf, sizeof("SEND_AUTHORIZATION"));
      HDLC_ready_to_send = true;
			Status = WAIT_AUTHORIZATION_ANSWER;
      break;
      // отправка сообщения авторизации
    case WAIT_AUTHORIZATION_ANSWER:
			memcpy(HDLC_GetBuf, getBuf, lenGetBuf);
			HDLC_ready_to_read = false;
			if(HDLC_GetBuf[0] == 'a')
			{
				HDLC_ready_to_read = true;
				Status = SEND_CONFIG_PARAM;
				memcpy(HDLC_SendBuf, "OK", sizeof("OK"));
				memcpy(sendBuf, HDLC_SendBuf, sizeof("OK"));
				HDLC_ready_to_send = true;
			}
			else
			{
				memcpy(HDLC_SendBuf, "ERROR", sizeof("ERROR"));
				memcpy(sendBuf, HDLC_SendBuf, sizeof("ERROR"));
				HDLC_ready_to_send = true;
			}
			
      break;
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
  memset(HDLC_SendBuf, 0, SEND_BUF_SIZE);
}
// ----------------------------------------------------------------------------
void ProtocolPackAuthorization(uint8_t pasword)
{
	memcmp(HDLC_SendBuf, authorize_msg, sizeof(authorize_msg));
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
