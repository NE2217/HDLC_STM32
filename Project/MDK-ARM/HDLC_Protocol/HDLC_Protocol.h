#ifndef _PROTOCOL_
#define _PROTOCOL_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

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


void ProtocolMain(void);
void HDLC_ProtocolMain(uint8_t* sendBuf, uint16_t lenSendBuf, uint8_t* getBuf, uint16_t lenGetBuf);

void ProtocolInitParamsStructureReset(t_InitParams *init);
void ProtocolInit(t_InitParams *init);
void DataReceive(uint8_t* data, uint16_t len);

float GetVoltageA(void);
float GetVoltageB(void);
float GetVoltageC(void);
float GetCurrentA(void);
float GetCurrentB(void);
float GetCurrentC(void);
float GetPowerA(void);
float GetPowerB(void);
float GetPowerC(void);
float GetEnergyActiveImport(void);
float GetEnergyActiveExport(void);
float GetEnergyReactiveImport(void);
float GetEnergyReactiveExport(void);

#endif // !_PROTOCOL_
