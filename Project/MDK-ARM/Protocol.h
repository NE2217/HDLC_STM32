#ifndef _PROTOCOL_
#define _PROTOCOL_

#include <iostream>
#include <stdint.h>

typedef struct
{
	uint32_t max_cadr_reception_data;
	uint32_t max_cadr_transmission_data;
	uint32_t max_window_reception_data;
	uint32_t max_window_transmission_data;
}t_InitParams;

void ProtocolMain();
void Init();
void DataReceive(uint8_t* data);
float f_GetVoltage(uint8_t phase);
float f_GetCurrent(uint8_t phase);
float f_GetPower(uint8_t phase);
float f_GetEnergy(uint8_t type);

#endif // !_PROTOCOL_