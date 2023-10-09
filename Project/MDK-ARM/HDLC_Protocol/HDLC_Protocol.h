#ifndef _PROTOCOL_
#define _PROTOCOL_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define DEFAULT_SERVER_ADDRESS    0x244C9
#define DEFAULT_CLIENT_ADDRESS    0x41
#define DEFAULT_PASWORD           "12345678"

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

bool IsConnect(void); // TODO добавить функцию определения наличия связи

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
