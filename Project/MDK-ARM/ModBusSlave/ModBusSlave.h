#ifndef MOD_BUS_SLAVE_H
#define MOD_BUS_SLAVE_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct
{
	uint8_t stationId;			// ����� ����������
	uint32_t currentTimeMks;	// ��������� �����
	uint32_t sielentTimeMks;	// ���������� ����� ������
	uint32_t sielentTimeMaxMks;	// ������������ �������� ������� ������
	
	uint16_t *HoldingRegistersPtr;
	uint16_t HoldingRegistersSize;

	uint16_t *InputRegistersPtr;
	uint16_t InputRegistersSize;
	
	bool *CoilStatusRegistersPtr;
	uint16_t CoilStatusRegistersSize;

	bool *InputStatusRegistersPtr;
	uint16_t InputStatusRegistersSize;
	
	// ������ ��� ������ ���������
	bool *CoilForcesRegistersPtr;
	uint16_t CoilForcesRegistersSize;	
	
	// ������ ��� ������ 16-������ ���������
	uint16_t *PresetRegistersPtr;
	uint16_t PresetRegistersSize;
	
	uint32_t (*_GetTicks100Mks)(void);		// ��������� �� ������� ������� �������
	void (*_sendByte)(uint8_t *data, uint16_t dataSize);	// ��������� �� ������� �������� �����
} t_modbus_init_struct;

void ModBusStructInit(t_modbus_init_struct * initStruct);
void ModBusInit(t_modbus_init_struct * initStruct);
void ModBusSetData(uint8_t *dataIn, uint16_t dataSize);
void ModBusRun(void);

#endif /* MOD_BUS_SLAVE_H */
