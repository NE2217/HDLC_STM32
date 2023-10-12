#include "ModBusSlave.h"


#define SIELENT_TIME_100MKS	40u	// время тишины кратное 100 мкc TODO дать подходящее имя

#define CMDReadCoilStatus					0x01	// чтение одного дискретного входа(DO_com)
#define CMDReadInputStatus					0x02	// чтение значений из нескольких дискретных входов (Read Discrete Inputs - DI_com).
#define CMDReadHoldingRegisters				0x03	// чтение значений из нескольких регистров хранениЯ (Read Holding Registers).
#define CMDReadInputRegisters				0x04	// чтение значений из нескольких регистров ввода (Read Input Registers).
#define CMDForceSingleCoil					0x05	// запись значениЯ одного флага (Force Single Coil). ED - Extern Digitals
#define CMDPresetSingleRegister				0x06	// запись значениЯ в один регистр хранениЯ (Preset Single Register). EA - Extern 
//#define CMDReadExceptionStatus				0x07	// чтение сигналов состояния (Read Exception Status)
#define CMDForceMultipleCoils				0x0F	// запись значений в несколько регистров флагов (Force Multiple Coils)
#define CMDPresetMultipleRegisters			0x10	// запись значений в несколько регистров хранения (Preset Multiple Registers)
//#define CMDMaskWriteRegister				0x16	// запись в один регистр хранениЯ с использованием маски З€И и маски З€‹€И (Mask Write Register).
//#define CMDReadFIFOQueue					0x18	// чтение данных из очереди (Read FIFO Queue)
//#define CMDReadFileRecord					0x14	// чтение из файла (Read File Record)
//#define CMDWriteFileRecord					0x15	// запись в файл (Write File Record)
//#define CMDDiagnostic						0x08	// диагностика (Diagnostic)
//#define CMDGetComEventCounter				0x0B	// чтение счетчика событий (Get Com Event Counter)
//#define CMDGetComEventLog					0x0C	// чтение журнала событий (Get Com Event Log)
//#define CMDReportSlaveID					0x11	// чтение информации об устройстве (Report Slave ID)
//#define CMDEncapsulatedInterfaceTransport	0x2B	// Encapsulated Interface Transport

// коды ошибок
#define ERROR_FUNCTION_CODE_CANNOT_PROCESSED	(uint8_t)0x01 //Принятый код функции не может быть обработан.
#define ERROR_DATA_ADDRESS_NOT_AVAILABLE		(uint8_t)0x02 //Адрес данных, указанный в запросе, недоступен.
#define ERROR_REQUEST_VALUE_NOT_VALID			(uint8_t)0x03 //Значение, содержащееся в поле данных запроса, является недопустимой величиной.
//04	Невосстанавливаемая ошибка имела место, пока ведомое устройство пыталось выполнить затребованное действие.
//05	Ведомое устройство приняло запрос и обрабатывает его, но это требует много времени. Этот ответ предохраняет ведущее устройство от генерации ошибки тайм-аута.
//06	Ведомое устройство занято обработкой команды. Ведущее устройство должно повторить сообщение позже, когда ведомое освободится.
//07	Ведомое устройство не может выполнить программную функцию, заданную в запросе. Этот код возвращается для неуспешного программного запроса, использующего функции с номерами 13 или 14. Ведущее устройство должно запросить диагностическую информацию или информацию об ошибках от ведомого.
//08	Ведомое устройство при чтении расширенной памяти обнаружило ошибку паритета. Ведущее устройство может повторить запрос, но обычно в таких случаях требуется ремонт.
//10(0A hex)	Шлюз неправильно настроен или перегружен запросами.
//11(0B hex)	Slave устройства нет в сети или от него нет ответа.

#define Uint16ByteSwap(v)	(uint16_t)((v<<8) | (v>>8))


#pragma pack(push, 1)	// установка выравнивания в 1 байт
typedef struct
{	
	uint8_t stationId;
	uint8_t functionCode;
	
	uint16_t firstRegAddress;
	uint16_t registersCount;
	uint16_t data;
} t_IncommingDataStruct;
#pragma pack(pop)

#pragma pack(push, 1)	// установка выравнивания в 1 байт
typedef struct
{	
	uint8_t stationId;
	uint8_t functionCode;
	
	uint16_t firstRegAddress;
	uint16_t registersCount;
	uint8_t dataBytesCount;
	uint8_t firstDataByte;
} t_IncommingDataStructForceMultipleCoils;
#pragma pack(pop)

#define RX_BUF_SIZE			100u
uint16_t rxBufCnt;
uint8_t RxBufModBus[RX_BUF_SIZE]; //
#define TX_BUF_SIZE			100u
uint16_t txBufCnt;
uint8_t TxBufModBus[TX_BUF_SIZE]; //

bool DataIsAccepted;	// состояние приема//неприема данных

t_modbus_init_struct ModBusHandler;


void ModBusSendData(uint8_t *data, uint16_t size);
void DataHandler(uint8_t *txBuf, uint16_t txBufSize, uint8_t *rxData, uint16_t rxDatasize);

uint16_t ReadCoilStatus(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData);
uint16_t ReadInputStatus(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData);
uint16_t ReadHoldingRegisters(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData);
uint16_t ReadInputRegisters(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData);
uint16_t ForceSingleCoil(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData);
uint16_t ForceMultipleCoils(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData);
uint16_t PresetSingleRegister(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData);
uint16_t PresetMultipleRegisters(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData);

uint16_t ErrorRequest(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData, uint8_t errorCode);



// очистка структуры перед использованием
void ModBusStructInit(t_modbus_init_struct *initStruct)
{
	memset(initStruct, 0, sizeof(t_modbus_init_struct));
}
// ----------------------------------------------------------------------------
// функция инициализации
void ModBusInit(t_modbus_init_struct * initStruct)
{
	ModBusStructInit(&ModBusHandler);

	ModBusHandler.stationId = initStruct->stationId;
	ModBusHandler._GetTicks100Mks = initStruct->_GetTicks100Mks;
	ModBusHandler._sendByte = initStruct->_sendByte;

	if(ModBusHandler._GetTicks100Mks != NULL)
	{ ModBusHandler.currentTimeMks = ModBusHandler._GetTicks100Mks(); }
	
	ModBusHandler.sielentTimeMks = 0;
	ModBusHandler.sielentTimeMaxMks = SIELENT_TIME_100MKS;
	
	ModBusHandler.HoldingRegistersPtr = initStruct->HoldingRegistersPtr;
	ModBusHandler.HoldingRegistersSize = initStruct->HoldingRegistersSize;

	ModBusHandler.InputRegistersPtr = initStruct->InputRegistersPtr;
	ModBusHandler.InputRegistersSize = initStruct->InputRegistersSize;
	
	ModBusHandler.CoilStatusRegistersPtr = initStruct->CoilStatusRegistersPtr;
	ModBusHandler.CoilStatusRegistersSize = initStruct->CoilStatusRegistersSize;
	
	ModBusHandler.InputStatusRegistersPtr = initStruct->InputStatusRegistersPtr;
	ModBusHandler.InputStatusRegistersSize = initStruct->InputStatusRegistersSize;

	ModBusHandler.CoilForcesRegistersPtr = initStruct->CoilForcesRegistersPtr;
	ModBusHandler.CoilForcesRegistersSize = initStruct->CoilForcesRegistersSize;
	
	ModBusHandler.PresetRegistersPtr = initStruct->PresetRegistersPtr;
	ModBusHandler.PresetRegistersSize = initStruct->PresetRegistersSize;
	
	rxBufCnt = 0;
	memset(RxBufModBus, 0, RX_BUF_SIZE);
	memset(TxBufModBus, 0, TX_BUF_SIZE);
	
	DataIsAccepted = false;
}
// ----------------------------------------------------------------------------
// вызов этой функции должен происходить по событию приема данных
void ModBusSetData(uint8_t *dataIn, uint16_t dataSize)
{
	if(ModBusHandler._GetTicks100Mks != NULL)
	{ ModBusHandler.currentTimeMks = ModBusHandler._GetTicks100Mks(); }

	if(rxBufCnt < RX_BUF_SIZE)
	{ RxBufModBus[rxBufCnt++] = *dataIn; }

	DataIsAccepted = true;
}
// ----------------------------------------------------------------------------
// эта функция должна вызываться в основном потоке
void ModBusRun(void)
{
	uint32_t tmp;

	if(ModBusHandler._GetTicks100Mks != NULL)
	{ 
		if(DataIsAccepted)
		{
			tmp = ModBusHandler._GetTicks100Mks();
			ModBusHandler.sielentTimeMks = (tmp - ModBusHandler.currentTimeMks);
			
			if( ModBusHandler.sielentTimeMks > ModBusHandler.sielentTimeMaxMks)
			{
				DataIsAccepted = false;
				DataHandler(TxBufModBus, TX_BUF_SIZE, RxBufModBus, rxBufCnt);
				
				rxBufCnt = 0;
			}
		}	
	}
}
// ----------------------------------------------------------------------------
uint16_t CalcCrc16(uint8_t *data, uint16_t size)
{
	uint16_t crc = 0xFFFF;
	uint8_t pos;
	uint8_t bitCnt;
	
	for(pos = 0; pos < size; ++pos)
	{
		crc ^= (uint16_t)data[pos];				// XOR byte into least sig. byte of crc
		
		for(bitCnt = 8; bitCnt |= 0; --bitCnt)	// Loop over each bit
		{
			if( (crc & 0x0001) != 0 )			// If the LSB is set
			{
				crc >>= 1;						// Shift right and XOR 0xA001
				crc ^= 0xA001;
			}
			else								// Else LSB is not set
			{
				crc >>= 1;						// Just shift right
			}
		}	
	}
	return crc;
}
// ----------------------------------------------------------------------------
bool CeckCrc16(uint8_t *data, uint16_t size)
{
	uint16_t crc;
	
	if(size < 2)
		return false;
	
	crc = (data[size-1]<<8) | (data[size-2]);
	
	return (crc == CalcCrc16(data, size-2));
}
// ----------------------------------------------------------------------------
void DataHandler(uint8_t *txBuf, uint16_t txBufSize, uint8_t *rxData, uint16_t rxDatasize)
{
	uint16_t countDataToSend = false;

	if(rxData[0] == ModBusHandler.stationId)
	{
		// проверка CRC
		if(!CeckCrc16(rxData, rxDatasize) )
			return;
		
		switch(rxData[1])
		{
			case CMDReadCoilStatus:	// чтение одного дискретного входа
				countDataToSend = ReadCoilStatus(TxBufModBus, TX_BUF_SIZE, rxData);
				break;
			case CMDReadInputStatus:	// чтение значений из нескольких дискретных входов (Read Discrete Inputs).
				countDataToSend = ReadInputStatus(TxBufModBus, TX_BUF_SIZE, rxData);
				break;
			case CMDReadHoldingRegisters:	// чтение значений из нескольких регистров хранениЯ (Read Holding Registers).
				countDataToSend = ReadHoldingRegisters(TxBufModBus, TX_BUF_SIZE, rxData); // SA_com registers
				break;
			case CMDReadInputRegisters:	// чтение значений из нескольких регистров ввода (Read Input Registers).
				countDataToSend = ReadInputRegisters(TxBufModBus, TX_BUF_SIZE, rxData); // AI_com registers
				break;
			case CMDForceSingleCoil:	// запись значениЯ одного флага (Force Single Coil).
				countDataToSend = ForceSingleCoil(TxBufModBus, TX_BUF_SIZE, rxData);
				break;
			case CMDPresetSingleRegister:	// запись значениЯ в один регистр хранениЯ (Preset Single Register).
				countDataToSend = PresetSingleRegister(TxBufModBus, TX_BUF_SIZE, rxData);
				break;
//			case CMDReadExceptionStatus:	// чтение сигналов состоЯниЯ (Read Exception Status)

//				break;
			case CMDForceMultipleCoils:	// запись значений в несколько регистров флагов (Force Multiple Coils)
				countDataToSend = ForceMultipleCoils(TxBufModBus, TX_BUF_SIZE, rxData);
				break;
			case CMDPresetMultipleRegisters:	// запись значений в несколько регистров хранениЯ (Preset Multiple Registers)
				countDataToSend = PresetMultipleRegisters(TxBufModBus, TX_BUF_SIZE, rxData);
				break;
//			case CMDMaskWriteRegister:	// запись в один регистр хранениЯ с использованием маски З€И и маски З€‹€И (Mask Write Register).

//				break;
//			case CMDReadFIFOQueue:	// чтение данных из очереди (Read FIFO Queue)

//				break;
//			case CMDReadFileRecord:	// чтение из файла (Read File Record)

//				break;
//			case CMDWriteFileRecord:	// запись в файл (Write File Record)

//				break;
//			case CMDDiagnostic:	// диагностика (Diagnostic)

//				break;
//			case CMDGetComEventCounter:	// чтение счетчика событий (Get Com Event Counter)

//				break;
//			case CMDGetComEventLog:	// чтение журнала событий (Get Com Event Log)

//				break;
//			case CMDReportSlaveID:	// чтение информации об устройстве (Report Slave ID)

//				break;
//			case CMDEncapsulatedInterfaceTransport:	// Encapsulated Interface Transport

//				break;
			default:
				countDataToSend = ErrorRequest(TxBufModBus, TX_BUF_SIZE, rxData, ERROR_FUNCTION_CODE_CANNOT_PROCESSED);
				break;
		}
	}
	if(countDataToSend > 0)
	{
		// отправка данных
		ModBusSendData(TxBufModBus, countDataToSend);
	}
}
// ----------------------------------------------------------------------------
uint16_t ReadRegisters(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{
	uint16_t txDataLen = 0;
	uint16_t firstRegIndex;
	uint16_t calledRegsMaxIndex;	// максимальный запрашиваемый индекс регистра
	uint16_t registerMaxIndex = 0;	// максимальный индекс регистра
	uint16_t *regsArrayPrt = NULL;	// указатель на массив регистров
	uint16_t crc16 = 0;
	
	t_IncommingDataStruct *dataStruct;
	dataStruct = (t_IncommingDataStruct *)rxData;
	
	dataStruct->stationId = dataStruct->stationId;
	dataStruct->firstRegAddress = Uint16ByteSwap(dataStruct->firstRegAddress);
	dataStruct->registersCount = Uint16ByteSwap(dataStruct->registersCount);

	calledRegsMaxIndex = (dataStruct->firstRegAddress + dataStruct->registersCount);
	
	if(dataStruct->functionCode == CMDReadHoldingRegisters)
	{ 
		registerMaxIndex = ModBusHandler.HoldingRegistersSize;
		regsArrayPrt = ModBusHandler.HoldingRegistersPtr;
	}
	if(dataStruct->functionCode == CMDReadInputRegisters)
	{ 
		registerMaxIndex = ModBusHandler.InputRegistersSize; 
		regsArrayPrt = ModBusHandler.InputRegistersPtr;
	}

	if(dataStruct->firstRegAddress >= registerMaxIndex)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_DATA_ADDRESS_NOT_AVAILABLE); }
	if(calledRegsMaxIndex > registerMaxIndex)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_REQUEST_VALUE_NOT_VALID); }
	
	txData[txDataLen++] = dataStruct->stationId;
	txData[txDataLen++] = dataStruct->functionCode;
	txData[txDataLen++] = dataStruct->registersCount * 2;
	
	for(firstRegIndex = dataStruct->firstRegAddress; firstRegIndex < calledRegsMaxIndex; ++firstRegIndex)
	{
		txData[txDataLen++] = regsArrayPrt[firstRegIndex]>>8;
		txData[txDataLen++] = regsArrayPrt[firstRegIndex];
	}
	crc16 = CalcCrc16(txData,txDataLen);
	txData[txDataLen++] = crc16 & 0xFF;
	txData[txDataLen++] = crc16>>8;
	
	return txDataLen;
}
// ----------------------------------------------------------------------------
uint16_t ReadHoldingRegisters(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{	
	return ReadRegisters(txData, txBufSize, rxData);
}
// ----------------------------------------------------------------------------
uint16_t ReadInputRegisters(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{
	return ReadRegisters(txData, txBufSize, rxData);
}
// ----------------------------------------------------------------------------
uint16_t ReadDiscrets(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{
	uint16_t txDataLen = 0;
	uint16_t firstRegIndex;
	uint8_t bitsCounter = 0;		// счетчик для формирования битовых байтов :)
	uint16_t calledRegsMaxIndex;	// максимальный запрашиваемый индекс регистра
	uint16_t registerMaxIndex = 0;	// максимальный индекс регистра
	bool *regsArrayPrt = NULL;		// указатель на массив регистров
	uint16_t crc16 = 0;
	
	t_IncommingDataStruct *dataStruct;
	dataStruct = (t_IncommingDataStruct *)rxData;
	
	dataStruct->stationId = dataStruct->stationId;
	dataStruct->firstRegAddress = Uint16ByteSwap(dataStruct->firstRegAddress);
	dataStruct->registersCount = Uint16ByteSwap(dataStruct->registersCount);

	calledRegsMaxIndex = (dataStruct->firstRegAddress + dataStruct->registersCount);
	
	if(dataStruct->functionCode == CMDReadCoilStatus)
	{ 
		registerMaxIndex = ModBusHandler.CoilStatusRegistersSize;
		regsArrayPrt = ModBusHandler.CoilStatusRegistersPtr;
	}
	if(dataStruct->functionCode == CMDReadInputStatus)
	{ 
		registerMaxIndex = ModBusHandler.InputStatusRegistersSize;
		regsArrayPrt = ModBusHandler.InputStatusRegistersPtr;
	}

	if(regsArrayPrt == NULL)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_FUNCTION_CODE_CANNOT_PROCESSED); }
	if(dataStruct->firstRegAddress >= registerMaxIndex)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_DATA_ADDRESS_NOT_AVAILABLE); }
	if(calledRegsMaxIndex > registerMaxIndex)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_REQUEST_VALUE_NOT_VALID); }
	
	txData[txDataLen++] = dataStruct->stationId;
	txData[txDataLen++] = dataStruct->functionCode;

	txData[txDataLen] = dataStruct->registersCount/8; // количество байт
	txData[txDataLen++] += ((dataStruct->registersCount%8) > 0)?(1):(0);
	
	bitsCounter = 0;
	txData[txDataLen] = 0;
	for(firstRegIndex = dataStruct->firstRegAddress; firstRegIndex < calledRegsMaxIndex; ++firstRegIndex)
	{
		if(bitsCounter > 7)
		{
			bitsCounter = 0;
			++txDataLen;
			txData[txDataLen] = 0;
		}
		txData[txDataLen] |= regsArrayPrt[firstRegIndex]<<bitsCounter;
		++bitsCounter;
	}
	++txDataLen;

	crc16 = CalcCrc16(txData,txDataLen);
	txData[txDataLen++] = crc16 & 0xFF;
	txData[txDataLen++] = crc16>>8;
	
	return txDataLen;
}
uint16_t ReadCoilStatus(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{
	return ReadDiscrets(txData, txBufSize, rxData);
}
uint16_t ReadInputStatus(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{
	return ReadDiscrets(txData, txBufSize, rxData);
}
// ----------------------------------------------------------------------------
uint16_t ForceSingleCoil(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{
	uint16_t txDataLen = 0;
//	uint16_t firstRegIndex;
//	uint8_t bitsCounter = 0;	// счетчик для формирования битовых байтов :)
//	uint16_t calledRegsMaxIndex;	// максимальный запрашиваемый индекс регистра
	uint16_t registerMaxIndex = 0;	// максимальный индекс регистра
	bool *regsArrayPrt = NULL;	// указатель на массив регистров
	uint16_t crc16 = 0;
	
	t_IncommingDataStruct *dataStruct;
	dataStruct = (t_IncommingDataStruct *)rxData;
	
	dataStruct->stationId = dataStruct->stationId;
	dataStruct->firstRegAddress = Uint16ByteSwap(dataStruct->firstRegAddress); 
	dataStruct->registersCount = Uint16ByteSwap(dataStruct->registersCount); // Coil set/reset value

	//calledRegsMaxIndex = (dataStruct->firstRegAddress + dataStruct->registersCount);
	
	if(dataStruct->functionCode == CMDForceSingleCoil)//CMDForceMultipleCoils
	{ 
		regsArrayPrt = ModBusHandler.CoilForcesRegistersPtr;
		registerMaxIndex = ModBusHandler.CoilForcesRegistersSize;
	}

	if(regsArrayPrt == NULL)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_FUNCTION_CODE_CANNOT_PROCESSED); }
	// проверка если адрес входит в диапазон адресов
	if(dataStruct->firstRegAddress >= registerMaxIndex)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_DATA_ADDRESS_NOT_AVAILABLE); }
	
	txData[txDataLen++] = dataStruct->stationId;
	txData[txDataLen++] = dataStruct->functionCode;

	txData[txDataLen++] = dataStruct->firstRegAddress>>8;
	txData[txDataLen++] = dataStruct->firstRegAddress & 0xFF;
	
	regsArrayPrt[dataStruct->firstRegAddress] = (dataStruct->registersCount == 0xFF00)?(true):(false);
	
	// Coil set/reset value
	txData[txDataLen++] = dataStruct->registersCount>>8;
	txData[txDataLen++] = dataStruct->registersCount & 0xFF;

	crc16 = CalcCrc16(txData,txDataLen);
	txData[txDataLen++] = crc16 & 0xFF;
	txData[txDataLen++] = crc16>>8;
	
	return txDataLen;
}
// ----------------------------------------------------------------------------
uint16_t ForceMultipleCoils(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{
	uint16_t txDataLen = 0;
	uint16_t writebleRegIndex;
	uint8_t bitsCounter = 0;		// счетчик для формирования битовых байтов :)
	uint16_t calledRegsMaxIndex;	// максимальный запрашиваемый индекс регистра
	uint16_t registerMaxIndex = 0;	// максимальный индекс регистра
	bool *regsArrayPrt = NULL;		// указатель на массив регистров
	uint8_t *bytesArrayPtr;			// указатель на данные во входящих данных
	uint16_t crc16 = 0;
	
	
	t_IncommingDataStructForceMultipleCoils *dataStruct;
	dataStruct = (t_IncommingDataStructForceMultipleCoils *)rxData;
	
	dataStruct->stationId = dataStruct->stationId;
	dataStruct->firstRegAddress = Uint16ByteSwap(dataStruct->firstRegAddress); 
	dataStruct->registersCount = Uint16ByteSwap(dataStruct->registersCount);

	// максимальный номер крайнего запрашиваемого регистра с учетом количества регистров
	calledRegsMaxIndex = (dataStruct->firstRegAddress + dataStruct->registersCount);
	
	if(dataStruct->functionCode == CMDForceMultipleCoils)//CMDForceSingleCoil
	{ 
		regsArrayPrt = ModBusHandler.CoilForcesRegistersPtr;
		registerMaxIndex = ModBusHandler.CoilForcesRegistersSize;
	}
	
	if(regsArrayPrt == NULL)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_FUNCTION_CODE_CANNOT_PROCESSED); }
	// проверка индекса первого запрашиваемого регистра, что он входит в запрашиваемый диапазон
	if(dataStruct->firstRegAddress > registerMaxIndex)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_DATA_ADDRESS_NOT_AVAILABLE); }
	// проверка, что индекс крайнего запрашиаемого регистра входит в диапазон регистров
	if(calledRegsMaxIndex > registerMaxIndex)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_REQUEST_VALUE_NOT_VALID); }
	
	txData[txDataLen++] = dataStruct->stationId;
	txData[txDataLen++] = dataStruct->functionCode;

	txData[txDataLen++] = dataStruct->firstRegAddress>>8;
	txData[txDataLen++] = dataStruct->firstRegAddress & 0xFF;
	
	bytesArrayPtr = &dataStruct->firstDataByte;
	for(writebleRegIndex = dataStruct->firstRegAddress; writebleRegIndex < calledRegsMaxIndex; ++writebleRegIndex)
	{	
		regsArrayPrt[writebleRegIndex] = ((*bytesArrayPtr) & (1<<bitsCounter)) ? (true) : (false);
		++bitsCounter;
		if(bitsCounter == 8)
		{ 
			bitsCounter = 0;
			++bytesArrayPtr;
		}
	}
	
	// Coil set/reset value
	txData[txDataLen++] = dataStruct->registersCount>>8;
	txData[txDataLen++] = dataStruct->registersCount & 0xFF;

	crc16 = CalcCrc16(txData,txDataLen);
	txData[txDataLen++] = crc16 & 0xFF;
	txData[txDataLen++] = crc16>>8;
	
	return txDataLen;
}
// ----------------------------------------------------------------------------
uint16_t PresetRegisters(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{
	uint16_t txDataLen = 0;
	uint16_t writebleRegIndex;
	uint16_t calledRegsMaxIndex;	// максимальный запрашиваемый индекс регистра
	uint16_t registerMaxIndex = 0;	// максимальный индекс регистра
	uint16_t *regsArrayPrt = NULL;		// указатель на массив регистров
	uint8_t *bytesArrayPtr;			// указатель на данные во входящих данных
	uint16_t crc16 = 0;
	
	
	t_IncommingDataStructForceMultipleCoils *dataStruct;
	dataStruct = (t_IncommingDataStructForceMultipleCoils *)rxData;
	
	dataStruct->stationId = dataStruct->stationId;
	dataStruct->firstRegAddress = Uint16ByteSwap(dataStruct->firstRegAddress); 
//	dataStruct->registersCount = Uint16ByteSwap(dataStruct->registersCount);

	// максимальный номер крайнего запрашиваемого регистра с учетом количества регистров
	if(dataStruct->functionCode == CMDPresetSingleRegister)
	{
		calledRegsMaxIndex = dataStruct->firstRegAddress + 1;
		bytesArrayPtr = (uint8_t*)&dataStruct->registersCount;
	}
	if(dataStruct->functionCode == CMDPresetMultipleRegisters)
	{
		dataStruct->registersCount = Uint16ByteSwap(dataStruct->registersCount);
		calledRegsMaxIndex = (dataStruct->firstRegAddress + dataStruct->registersCount);
		bytesArrayPtr = &dataStruct->firstDataByte;
	}
	
	regsArrayPrt = ModBusHandler.PresetRegistersPtr;
	registerMaxIndex = ModBusHandler.PresetRegistersSize;
	
	if(regsArrayPrt == NULL)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_FUNCTION_CODE_CANNOT_PROCESSED); }
	// проверка, что индекс крайнего запрашиаемого регистра входит в диапазон регистров
	if(calledRegsMaxIndex > registerMaxIndex)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_REQUEST_VALUE_NOT_VALID); }
	// проверка индекса первого запрашиваемого регистра, что он входит в запрашиваемый диапазон
	if(dataStruct->firstRegAddress > registerMaxIndex)
	{ return ErrorRequest(txData, txBufSize, rxData, ERROR_DATA_ADDRESS_NOT_AVAILABLE); }
	
	txData[txDataLen++] = dataStruct->stationId;
	txData[txDataLen++] = dataStruct->functionCode;

	txData[txDataLen++] = dataStruct->firstRegAddress>>8;
	txData[txDataLen++] = dataStruct->firstRegAddress & 0xFF;
	
	for(writebleRegIndex = dataStruct->firstRegAddress; writebleRegIndex < calledRegsMaxIndex; ++writebleRegIndex)
	{	
		regsArrayPrt[writebleRegIndex] = (uint16_t)(*bytesArrayPtr++)<<8;
		regsArrayPrt[writebleRegIndex] |= (uint16_t)((*(bytesArrayPtr++)) & 0xFF);
	}
	
	// Coil set/reset value
	if(dataStruct->functionCode == CMDPresetSingleRegister)
	{
		txData[txDataLen++] = regsArrayPrt[dataStruct->firstRegAddress]>>8;
		txData[txDataLen++] = regsArrayPrt[dataStruct->firstRegAddress] & 0xFF;		
	}
	if(dataStruct->functionCode == CMDPresetMultipleRegisters)
	{
		txData[txDataLen++] = dataStruct->registersCount>>8;
		txData[txDataLen++] = dataStruct->registersCount & 0xFF;
	}

	crc16 = CalcCrc16(txData,txDataLen);
	txData[txDataLen++] = crc16 & 0xFF;
	txData[txDataLen++] = crc16>>8;
	
	return txDataLen;	
}
// ----------------------------------------------------------------------------
uint16_t PresetSingleRegister(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{
	return PresetRegisters(txData, txBufSize, rxData);
}
// ----------------------------------------------------------------------------
uint16_t PresetMultipleRegisters(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData)
{
	return PresetRegisters(txData, txBufSize, rxData);
}
// ----------------------------------------------------------------------------
uint16_t ErrorRequest(uint8_t *txData, uint16_t txBufSize, uint8_t *rxData, uint8_t errorCode)
{
	uint16_t txDataLen = 0;
	uint16_t crc16 = 0;
	
	t_IncommingDataStruct *dataStruct = (t_IncommingDataStruct *)rxData;
	
	txData[txDataLen++] = dataStruct->stationId;
	txData[txDataLen++] = dataStruct->functionCode | 0x80;

	txData[txDataLen++] = errorCode;
	
	crc16 = CalcCrc16(txData,txDataLen);
	txData[txDataLen++] = crc16 & 0xFF;
	txData[txDataLen++] = crc16>>8;
	
	return txDataLen;
}
// ----------------------------------------------------------------------------
// здесь происходит отправка данных используя CallBack модуля отправки(Uart, Can, Ethernet и т.д.)
void ModBusSendData(uint8_t *txData, uint16_t txDataLen)
{
	if(ModBusHandler._sendByte != NULL)
	{ ModBusHandler._sendByte(txData, txDataLen); }
}
// ----------------------------------------------------------------------------
