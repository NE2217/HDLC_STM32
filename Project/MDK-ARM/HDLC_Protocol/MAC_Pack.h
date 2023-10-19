#ifndef MACK_PACK_H
#define MACK_PACK_H

#include <stdint.h>
#include "CRC.h"

#define FLAG                  0x7e  //флаг конца/начала пакета
#define HEADER_LEN              8    //байт, длинна заголовка
#define FORMAT_TYPE              0xA  //1010
#define FORMAT_S                0    //все данные в одном сообщении
#define CONTROL_FIELDS_SIZE      14  //размер информационных полей в байтах
//команды в окне формат  
  
//Разряды байта управляющего поля  Команда / ответ          Формат
//1   2  3  4  5  6  7  8    Тип кадра
//0     N(S)     P/F     N(R)      -                I   информационный
//1   0  0  0  P/F     N(R)    RR - готов к приему          S  супервизорный
//1   0  1  0  P/F     N(R)    RNR - не готов к приему
//1   0  0  1  P/F     N(R)    REJ - отказ в приеме        
//1   0  1  1  P/F     N(R)    SREJ - выборочный отказ в приеме
//1   1  0  0  P/F  0  0  0  UI - короткая информация      U  ненумерованный, управляющий
//1   1  0  0  Р  0  0  1  SNRM - режим нормального ответа
//1   1  0  0  Р  0  1  0  DISC - разъединить
//1   1  0  0  Р  1  0  0  UP - ненумерованный опрос
//1   1  0  0  F  1  1  0  UA - подтверждение
//1   1  0  0  P/F  1  1  1  TEST - проверка
//1   1  1  0  P/F  0  0  0  SIM - режим инициативного выхода
//1   1  1  0  F  0  0  1  FRMR - неприем кадра
//1   1  1  1  F  0  0  0  DM - "Разъединено"
//1   1   1  1  Р  0  0  1  RSET - сброс счетчика принятых кадров
//1   1  1  1  Р  0  1  0  SARME - режим длинного асинхронного ответа
//1   1  1  1  Р  0  1  1  SNRME - режим длинного нормального ответа
//1   1  1  1  Р  1  0  0  SABM - асинхронный сбалансированный режим
//1   1  1  1  P/F  1  0  1  XID - идентификация станции
//1   1  1  1  Р  1  1  0  SABME - режим длинного сбалансированного асинхронного ответа


//                      
//Маски команд (неопределенные биты нулевые)       Команда / ответ               Формат                        
#define Inf    0x0  //                    I   информационный

#define RR    0x1    //готов к приему          S  супервизорный
#define RNR    0x5    //не готов к приему
#define REJ    0x9    //отказ в приеме        
#define SREJ  0xD    //выборочный отказ в приеме

#define UI    0x3    //короткая информация        U  ненумерованный, управляющий
//#define SNRM  0x83  //режим нормального ответа
#define DISC  0x43  //разъединить
#define UP    0x23  //ненумерованный опрос
//#define UA    0x63  //подтверждение
#define TEST  0xC7  //проверка
#define SIM   0x7    //режим инициативного выхода
#define FRMR  0x87  //неприем кадра
#define DM    0xF    //"Разъединено"
#define RSET  0x8F  //сброс счетчика принятых кадров
#define CF    0x4F  //режим длинного асинхронного ответа
#define SNRME  0xCF  //режим длинного нормального ответа
#define SABM  0x2F  //асинхронный сбалансированный режим
#define XID    0xAF  //идентификация станции
#define SABME  0x6F  //режим длинного сбалансированного асинхронного ответа

#pragma pack(push,1)
#pragma anon_unions
// TODO это две структуры объявлены правильно, кроме битовых полей. Остальные нужно сделать также

typedef union {
  uint16_t point;
  struct {
    unsigned size : 11;  // TODO переделать все uint16_t в unsigned. Перепроверь все свои битовые поля
    unsigned S : 1;    //Ответ: При использовании unsigned, выделяется 4 байта под созданный тип
    unsigned typ : 4;
  }form;
}t_format;

typedef struct {
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

#pragma pack(pop)
#endif /*HDLC_PROTOCOL.h*/
