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

//--------------------------------------------------------------------------------
#define FLAG                    0x7e  //флаг конца/начала пакета
#define HEADER_LEN              8     //байт, длинна заголовка
#define FORMAT_TYPE             0xA   //1010
#define FORMAT_S                0     //все данные в одном сообщении
#define CONTROL_FIELDS_SIZE     14    //размер информационных полей в байтах
//команды в окне формат

//Разряды байта управляющего поля  Команда / ответ          Формат
//1   2   3   4   5   6   7   8    Тип кадра
//0      N(S)    P/F     N(R)                                      I   информационный
//1   0   0   0  P/F     N(R)    RR - готов к приему               S  супервизорный
//1   0   1   0  P/F     N(R)    RNR - не готов к приему
//1   0   0   1  P/F     N(R)    REJ - отказ в приеме        
//1   0   1   1  P/F     N(R)    SREJ - выборочный отказ в приеме
//1   1   0   0  P/F  0   0   0  UI - короткая информация          U  ненумерованный, управляющий
//1   1   0   0   Р   0   0   1  SNRM - режим нормального ответа
//1   1   0   0   Р   0   1   0  DISC - разъединить
//1   1   0   0   Р   1   0   0  UP - ненумерованный опрос
//1   1   0   0   F   1   1   0  UA - подтверждение
//1   1   0   0  P/F  1   1   1  TEST - проверка
//1   1   1   0  P/F  0   0   0  SIM - режим инициативного выхода
//1   1   1   0   F   0   0   1  FRMR - неприем кадра
//1   1   1   1   F   0   0   0  DM - "Разъединено"
//1   1   1   1   Р   0   0   1  RSET - сброс счетчика принятых кадров
//1   1   1   1   Р   0   1   0  SARME - режим длинного асинхронного ответа
//1   1   1   1   Р   0   1   1  SNRME - режим длинного нормального ответа
//1   1   1   1   Р   1   0   0  SABM - асинхронный сбалансированный режим
//1   1   1   1  P/F  1   0   1  XID - идентификация станции
//1   1   1   1   Р   1   1   0  SABME - режим длинного сбалансированного асинхронного ответа

//                      
//Маски команд (неопределенные биты нулевые)       Команда / ответ               Формат
                                      //I   информационный
#define I_FORM 0x0
#define S_FORM 0xF                    //S   супервизорный
#define RR     0x1   //готов к приему
#define RNR    0x5   //не готов к приему
#define REJ    0x9   //отказ в приеме
#define SREJ   0xD   //выборочный отказ в приеме
#define U_FORM 0xEF                   //U   ненумерованный, управляющий
#define UI     0x3   //короткая информация
#define SNRM   0x83  //режим нормального ответа
#define DISC   0x43  //разъединить
#define UP     0x23  //ненумерованный опрос
#define UA     0x63  //подтверждение
#define TEST   0xC7  //проверка
#define SIM    0x7   //режим инициативного выхода
#define FRMR   0x87  //неприем кадра
#define DM     0xF   //"Разъединено"
#define RSET   0x8F  //сброс счетчика принятых кадров
#define CF     0x4F  //режим длинного асинхронного ответа
#define SNRME  0xCF  //режим длинного нормального ответа
#define SABM   0x2F  //асинхронный сбалансированный режим
#define XID    0xAF  //идентификация станции
#define SABME  0x6F  //режим длинного сбалансированного асинхронного ответа

//--------------------------------------------------------------------------------
#pragma pack(push,1)
#pragma anon_unions

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

typedef struct
{
  uint8_t type;
  uint8_t data;
} t_cont;//формат контрольного слова для передачи

typedef union 
{
  uint16_t point;
  struct 
  {
    unsigned size : 11;  // TODO переделать все uint16_t в unsigned. Перепроверь все свои битовые поля
    unsigned S : 1;      //Ответ: При использовании unsigned, выделяется 4 байта под созданный тип
    unsigned typ : 4;
  } form;
} t_format;

typedef struct 
{
  uint8_t flag_open;
  t_format format;
  uint8_t DA_SA[5];//адрес назначения/источника
  uint8_t control;
  uint16_t HCS;//контрольная сумма загаловка
  uint8_t data;
}t_HDLC_packet_begin;

typedef struct
{
  uint16_t FCS;
  uint8_t flag_close;
} t_HDLC_packet_end;

typedef struct
{
  t_HDLC_packet_begin *begin;
  t_HDLC_packet_end *end;
} t_HDLC_packet;

typedef struct
{
  uint16_t Adr;
  uint8_t password[8];
} t_HDLCservParam;

#pragma pack ( pop )
#pragma no_anon_unions

uint8_t HDLC_ProtocolMain(void);

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

void HDLCservParamSET(t_HDLCservParam Param, uint8_t N_serv);
t_HDLCservParam HDLCservParamGET(uint8_t N_serv);
#endif // !_PROTOCOL_
