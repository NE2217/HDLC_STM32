#ifndef _PROTOCOL_
#define _PROTOCOL_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define DEFAULT_SERVER_ADDRESS                 0x244C9
#define DEFAULT_CLIENT_ADDRESS                 0x41
#define DEFAULT_PASWORD                        "12345678"
#define DEFAULT_MAX_CADR_RECEPTION_DATA        1024
#define DEFAULT_MAX_CADR_TRANSMISSION_DATA     1024
#define DEFAULT_MAX_WINDOW_RECEPTION_DATA      1024
#define DEFAULT_MAX_WINDOW_TRANSMISSION_DATA   1024
typedef struct
{
  uint32_t max_cadr_reception_data;
  uint32_t max_cadr_transmission_data;
  uint32_t max_window_reception_data;
  uint32_t max_window_transmission_data;
  uint32_t server_address;
  uint32_t client_address;
  uint8_t pasword[8];
  
  void (*uartSendDataCB)(uint8_t *data, uint16_t len);
  uint32_t (*getTicksCB)(void);
}t_InitParams;

void HDLC_ProtocolMain(void);

void HDLC_ProtocolInitParamsStructureReset(t_InitParams *init);
void HDLC_ProtocolInit(t_InitParams *init);
void HDLC_ProtocolDataReceive(uint8_t* data, uint16_t len);

bool IsConnect(void);

float GetVoltageA(void);
float GetVoltageB(void);
float GetVoltageC(void);
float GetCurrentA(void);
float GetCurrentB(void);
float GetCurrentC(void);
//float GetPowerA(void);
//float GetPowerB(void);
//float GetPowerC(void);
float GetSumPowerApparent(void);
float GetSumPowerActive(void);
float GetSumPowerReactive(void);
float GetCosFi(void);
int64_t GetEnergyActiveImport(void);
int64_t GetEnergyReactiveImport(void);
int64_t GetEnergyApparentImport(void);

#endif // !_PROTOCOL_
