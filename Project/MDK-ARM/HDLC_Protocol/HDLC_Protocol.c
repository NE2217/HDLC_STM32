#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "HDLC_Protocol.h"
#include "HDLC_unpack.h"
#include "HDLC_pack.h"

#define POLLING_TIME             200u
#define TIMEOUT                  2000u
#define SEND_BUF_SIZE            100u
#define GET_BUF_SIZE             100u
#define SEND_AUTHORIZATION_SIZE  100u
#define NUMBER_OF_PARAMETERS     13

typedef enum
{
  NONE = 0,
  FIRST_AUTHORIZATION,
  SEND_CONFIG_PARAM,
  WAIT_CONFIG_PARAM_ANSWER,
  SEND_AUTHORIZATION,
  WAIT_AUTHORIZATION_ANSWER,
  SEND_COMAND,
  WAIT_COMAND_ANSWER,
  SEND_DISCONNECT,
  WAIT_DISCONNECT_ANSWER
} t_protocol_status;

typedef enum
{
  BUFF_IS_EMPTY = 0,
  RECORDING_IN_PROGRESS,
  BUFF_IS_FULL,
  LIMIT_EXXCEDED
} t_GetBuf_status;

typedef struct
{
  uint8_t *point;
  uint16_t size;
} t_PointAndSize;

bool Connect;
bool Timeout = false;
bool WaitingForResponse = false;
float Volt = 0;
float Current = 0;
uint32_t LocalTime;
uint32_t StartTimeoutCounter;

t_GetBuf_status BufStat;
t_protocol_status Status;
t_InitParams Parameters;
//--------------------------------------------------------------------------------
float Rep_current_A;
float Rep_current_B;
float Rep_current_C;
float Rep_volt_A;
float Rep_volt_B;
float Rep_volt_C;
float Rep_sum_Q;
float Rep_sum_P;
float Rep_sum_S;
float Rep_cos_f;
int64_t Rep_activ_energy_import;
int64_t Rep_reactiv_energy_import;
int64_t Rep_apparent_energy_import;
//-----------------------------------Test----------------------------------------

//--------------------------------------------------------------------------------
void* Rep_Points[] = {&Rep_current_A, &Rep_current_B, &Rep_current_C, &Rep_volt_A, &Rep_volt_B, 
                      &Rep_volt_C, &Rep_sum_Q, &Rep_sum_P, &Rep_sum_S, &Rep_cos_f, &Rep_activ_energy_import, 
                      &Rep_reactiv_energy_import, &Rep_apparent_energy_import};
uint8_t Param_pos = 0;
//--------------------------------------------------------------------------------
uint8_t NRS=0;
uint8_t authorize_msg[] = {0,0,0,0,0,0};

uint8_t HDLC_SendBuf[SEND_BUF_SIZE];
uint8_t HDLC_GetBuf[GET_BUF_SIZE];

uint16_t GetBufHead;
bool RecordingInProgress;

void HDLC_ConnectAutorization(void);
void HDLC_SendConnectAutorization_t(uint8_t* SendBuf, void SendData(uint8_t* data, uint16_t data_len));
bool HDLC_GetConnectAutorization_t(uint8_t* GetBuf);

void TimeOutReset(void);
bool IsTimeOut(void);
void BufReset(void);
t_GetBuf_status IsBufstat(void);

bool HDLC_SendData(uint8_t *data, uint16_t len); 
uint32_t GetTicks(void);

bool HDLC_SendData(uint8_t *data, uint16_t len)
{
  if(Parameters.uartSendDataCB == NULL)
    return false;
  return true;
// сброс таймаута
}

uint32_t GetTicks(void)
{
  if(Parameters.getTicksCB == NULL)
    return 0;
  return Parameters.getTicksCB();
}
//----------------------------------------------------------------------------
void HDLC_ProtocolMain(void) 
{
  uint32_t time = Parameters.getTicksCB();

  if( (time - LocalTime) < POLLING_TIME )
    return;
  LocalTime = Parameters.getTicksCB();
  
  if( IsTimeOut() )
  {
    TimeOutReset();
    Status = SEND_DISCONNECT;
    WaitingForResponse = false;
    NRS = 0;
    Param_pos = 0;
    Connect = false;
  }

  if ( !WaitingForResponse || (IsBufstat() == BUFF_IS_FULL) )
  {
   switch(Status)
    {
      case NONE:
        HDLC_PackAdr(2544, 0x20);
        HDLC_UnPackAdr(2544, 0x20);
        Status = SEND_CONFIG_PARAM;
        TimeOutReset();
        break;
      case SEND_CONFIG_PARAM:
        HDLC_PackSendConfigParam(Parameters.uartSendDataCB);
        Status = WAIT_CONFIG_PARAM_ANSWER;
        TimeOutReset();
        WaitingForResponse = true;
        break;

      case WAIT_CONFIG_PARAM_ANSWER:
        if(HDLC_UnpackWaitConfigParam(HDLC_GetBuf, GET_BUF_SIZE) == 0)
        {
          Status = SEND_AUTHORIZATION;
          BufReset();
          WaitingForResponse = false;
        }
        else
          Status = NONE;
        break;
      case SEND_AUTHORIZATION:
        HDLC_PackSendAuthorization(Parameters.uartSendDataCB, NRS++);
        Status = WAIT_AUTHORIZATION_ANSWER;
        WaitingForResponse = true;
        TimeOutReset();
        break;
      case WAIT_AUTHORIZATION_ANSWER:
        if(HDLC_UnpackWaitAuthorization(HDLC_GetBuf, GET_BUF_SIZE) == 0)
        {
          Status = SEND_COMAND;
          BufReset();
          WaitingForResponse = false;
        }
        break;
      case SEND_COMAND:
        HDLC_PackSendComand(NRS++,Parameters.uartSendDataCB, Param_pos);
        Status =  WAIT_COMAND_ANSWER;
        TimeOutReset();
        WaitingForResponse = true;
        if (NRS>7) NRS=0;
        break;
      case WAIT_COMAND_ANSWER:
        HDLC_UnpackComand(Rep_Points[Param_pos++], HDLC_GetBuf, GET_BUF_SIZE);
        Status = SEND_COMAND;
        WaitingForResponse = false;
        BufReset();
        if(Param_pos > NUMBER_OF_PARAMETERS-1) Param_pos=0;
        break;
      case SEND_DISCONNECT:
        HDLC_PackDisconnect(Parameters.uartSendDataCB);
        Status = WAIT_DISCONNECT_ANSWER;
        TimeOutReset();
        WaitingForResponse = true;
        break;
      case WAIT_DISCONNECT_ANSWER:
        HDLC_UnpackDisconnect(HDLC_GetBuf, GET_BUF_SIZE);
        Status = NONE;
        WaitingForResponse = false;
        BufReset();
        break;
      default:
        break;
    }
  }
  // если данные отправляются и принимаются нормально, то сброс таймаута
}
// ----------------------------------------------------------------------------
//#define NRS_MASK 7u TODO
//#define NRS_RESET_VALUE  1u
//void ResetNRS(void)
//{ NRS = NRS_RESET_VALUE; }
//uint8_t GetNextNRS(void)
//{
//  return (++NRS & NRS_MASK);
//}
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
  
  Parameters.max_cadr_reception_data = DEFAULT_MAX_CADR_RECEPTION_DATA;
  Parameters.max_cadr_transmission_data = DEFAULT_MAX_CADR_TRANSMISSION_DATA;
  Parameters.max_window_reception_data = DEFAULT_MAX_WINDOW_RECEPTION_DATA;
  Parameters.max_window_transmission_data = DEFAULT_MAX_WINDOW_TRANSMISSION_DATA;
  
  Rep_current_A = 0;
  Rep_current_B = 0;
  Rep_current_C = 0;
  Rep_volt_A = 0;
  Rep_volt_B = 0;
  Rep_volt_C = 0;
  Rep_sum_Q = 0;
  Rep_sum_P = 0;
  Rep_sum_S = 0;
  Rep_cos_f = 0;
  Rep_activ_energy_import = 0;
  Rep_reactiv_energy_import = 0;
  Rep_apparent_energy_import = 0;
// TODO  NRS=1; 
//  void TimeOutReset(void);
}
//-----------------------------------------------------------------------------
void HDLC_ProtocolPackAuthorization(uint8_t pasword)
{
  memcmp(HDLC_SendBuf, authorize_msg, sizeof(authorize_msg));
}
//------------------------------CONNECT----------------------------------------
bool IsConnect(void)
{
  return Connect;
}

void BufReset(void)
{
  BufStat = BUFF_IS_EMPTY;
  GetBufHead = 0;
}

t_GetBuf_status IsBufstat(void)
{
  return BufStat;
}

void TimeOutReset(void)
{
  StartTimeoutCounter = Parameters.getTicksCB();
  Timeout = false;
}

bool IsTimeOut(void)
{
  uint32_t time = Parameters.getTicksCB();

  if( (time - StartTimeoutCounter) < TIMEOUT )
  {
    Timeout = false;
  }
  else
  {  
    Timeout = true;
  }
  return Timeout;
}
void HDLC_ProtocolDataReceive(uint8_t* data, uint16_t len)
{
  if( IsBufstat() == BUFF_IS_EMPTY )
  {
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
      BufStat = BUFF_IS_FULL;
      RecordingInProgress = false;
    }

    if(RecordingInProgress == true)
    {
      HDLC_GetBuf[ GetBufHead ] = data[i];
      GetBufHead++;
    }
   }
  }  
}
// --------------------------------Показания----------------------------------

float GetVoltageA(void)
{
  return Rep_volt_A;
}

float GetVoltageB(void)
{
  return Rep_volt_B;
}
float GetVoltageC(void)
{
  return Rep_volt_C;
}
float GetCurrentA(void)
{
  return Rep_current_A;
}
float GetCurrentB(void)
{
  return Rep_current_B;
}
float GetCurrentC(void)
{
  return Rep_current_C;
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
float GetSumPowerApparent(void)
{
  return Rep_sum_S;
}
float GetSumPowerActive(void)
{
  return Rep_sum_P;
}
float GetSumPowerReactive(void)
{
  return Rep_sum_Q;
}
float GetCosFi(void)
{
  return Rep_cos_f;
}
int64_t GetEnergyActiveImport(void)
{
  return Rep_activ_energy_import;
}
int64_t GetEnergyReactiveImport(void)
{
  return Rep_reactiv_energy_import;
}
int64_t GetEnergyApparentImport(void)
{
  return Rep_apparent_energy_import;
}
