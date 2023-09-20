#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "HDLC_Protocol.h"

#define POLLING_TIME						1000u
#define SEND_BUF_SIZE						100u
#define GET_BUF_SIZE						100u
#define SEND_AUTHORIZATION_SIZE	100u
#define FLAG										0x7E

static uint8_t send_1[] = { 0x7E, 0xA0, 0x23, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x93, 0x77, 0x28, 0x81, 0x80, 0x14, 0x05, 0x02,
0x04, 0x00, 0x06, 0x02, 0x04, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
0x00, 0x01, 0x72, 0xE3, 0x7E };

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

uint16_t GetBufHead;
bool RecordingInProgress;

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
  switch(Status)
  {
    case NONE:
			Status = SEND_AUTHORIZATION;
      break;
    case SEND_AUTHORIZATION:
			memcpy(HDLC_SendBuf, send_1, sizeof(send_1));
			Parameters.uartSendDataCB(HDLC_SendBuf, sizeof(send_1));
			//Status = WAIT_AUTHORIZATION_ANSWER;
      break;
      // отправка сообщения авторизации
    case WAIT_AUTHORIZATION_ANSWER:
//			memcpy(HDLC_GetBuf, getBuf, lenGetBuf);

//			if(HDLC_GetBuf[0] == 'a')
//			{

//				Status = SEND_CONFIG_PARAM;
//				memcpy(HDLC_SendBuf, "OK", sizeof("OK"));
//			}
//			else
//			{
//				memcpy(HDLC_SendBuf, "ERROR", sizeof("ERROR"));
//			}
			
      break;
      // отправка сообщения авторизации  
    default:
      break;
  }
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
}
// ----------------------------------------------------------------------------
void HDLC_ProtocolPackAuthorization(uint8_t pasword)
{
	memcmp(HDLC_SendBuf, authorize_msg, sizeof(authorize_msg));
}
// ----------------------------------------------------------------------------
void HDLC_ProtocolDataReceive(uint8_t* data, uint16_t len)
{
	for(int i = 0; i < len; ++i)
	{
		if(data[i] == FLAG)
		{
			RecordingInProgress = true;
		}
		if (RecordingInProgress == true)
		{
			HDLC_GetBuf[ GetBufHead ] = data[i];
			GetBufHead++;
			if(data[i] == FLAG)
			{
				RecordingInProgress = false;
			}
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
