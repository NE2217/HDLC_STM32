#ifndef _PROTOCOL_
#define _PROTOCOL_

#include <stdint.h>

typedef struct
{
	uint32_t max_cadr_reception_data;
	uint32_t max_cadr_transmission_data;
	uint32_t max_window_reception_data;
	uint32_t max_window_transmission_data;
	uint32_t server_address;
	uint32_t client_address;
	uint64_t pasword;
}t_InitParams;

void ProtocolMain();
void Init();
void DataReceive(uint8_t* data);

float GetVoltageA();
float GetVoltageB();
float GetVoltageC();
float GetCurrentA();
float GetCurrentB();
float GetCurrentC();
float GetPowerA();
float GetPowerB();
float GetPowerC();
float GetEnergyActiveImport();
float GetEnergyActiveExport();
float GetEnergyReactiveImport();
float GetEnergyReactiveExport();

#endif // !_PROTOCOL_