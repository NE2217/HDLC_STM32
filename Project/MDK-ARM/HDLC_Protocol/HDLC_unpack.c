#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "HDLC_unpack.h"
#include "MAC_Pack.h"
#include "CRC.h"
#include "HDLC_Protocol.h"
//--------------------------------------------------------------------------------
#define FLOAT      0x17
#define INT_64     0x14
typedef struct
{
//  uint8_t MAC_start [11];
  uint8_t res[3];
  uint8_t req_res;
  uint8_t res_2[3];
  uint8_t type;
  uint8_t value;
}t_answer;

typedef struct
{
  uint8_t MAC_start [11];
  uint8_t res[19];
  uint8_t result_connect;
}t_authorization_answer;
//--------------------------------TEST------------------------------------------------
uint8_t Test_i = 0;
uint8_t MAC_ERROR = 0;
//--------------------------------------------------------------------------------
uint8_t response_connect[] = { 0x7E, 0xA0, 0x23, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x73, 0xB9, 0x33, 0x81, 0x80, 0x14, 0x05, 0x02,
  0x04, 0x00, 0x06, 0x02, 0x04, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
  0x00, 0x01, 0x72, 0xE3, 0x7E };

uint8_t response_authorize[] = { 0x7E, 0xA0, 0x3A, 0x41, 0x00, 0x02, 0x44, 0xC9, 0x30, 0xD6, 0xB7, 0xE6, 0xE7, 0x00, 0x61, 0x29,
  0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xA2, 0x03, 0x02, 0x01, 0x00,
  0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00, 0xBE, 0x10, 0x04, 0x0E, 0x08, 0x00, 0x06, 0x5F, 0x1F,
  0x04, 0x00, 0x00, 0x10, 0x15, 0x04, 0x00, 0x00, 0x07, 0x62, 0xC5, 0x7E };
uint8_t DA_SA_response[] = {0x41, 0x00, 0x02, 0x44, 0xC9};
//--------------------------------------------------------------------------------
uint8_t f_MAC_unpack(t_cont* control_LLC, uint8_t** data, uint16_t* data_len, uint8_t* pack, uint16_t len); // Размер в байтах
uint8_t HDLC_UnpackWaitConfigParam(uint8_t* data, uint16_t len)
{
  t_HDLC_packet_begin* pack_begin = (t_HDLC_packet_begin*) data;
  
  /*
  if( memcmp(data, response_connect, sizeof(response_connect)) == 0 )
  {
    return 0;
  }
  */
  if (memcmp(pack_begin->DA_SA, DA_SA_response, sizeof(DA_SA_response)) == 0)
  {
    return 0;
  }
  return 1;
}
//--------------------------------------------------------------------------------
uint8_t HDLC_UnpackWaitAuthorization(uint8_t* data, uint16_t len)
{
  /*
  if( memcmp(data, response_authorize, sizeof(response_authorize)) == 0 )
  {
    return 0;
  }
  */
  t_authorization_answer* pack = (t_authorization_answer*) data;
  if (pack->result_connect == 1)
    return 0;
  return 1;
}
//--------------------------------------------------------------------------------
uint8_t HDLC_UnpackComand(void* Param, uint8_t* pack, uint16_t len)
{
  t_cont control;
  uint8_t* inf;
  uint16_t data_len;
  MAC_ERROR = f_MAC_unpack(&control, &inf, &data_len, pack, len);
  t_answer* data = (t_answer*)inf;
  
  if (data->type == FLOAT)
  {
    uint8_t rev[4] = {0};
    rev[3] = *(&data->value + 0);
    rev[2] = *(&data->value + 1);
    rev[1] = *(&data->value + 2);
    rev[0] = *(&data->value + 3);

    float* Par = Param;
    *Par = *( (float*) rev );
  }
  
  else if(data->type == INT_64)
  {
    uint8_t rev[8] = {0};
    rev[7] = *(&data->value + 0);
    rev[6] = *(&data->value + 1);
    rev[5] = *(&data->value + 2);
    rev[4] = *(&data->value + 3);
    rev[3] = *(&data->value + 4);
    rev[2] = *(&data->value + 5);
    rev[1] = *(&data->value + 6);
    rev[0] = *(&data->value + 7);
    
    int64_t* Par = Param;
    *Par = *( (int64_t*) rev );//(Test_i++)%13;
  }
  return 0;
}

void HDLC_UnPackAdr(uint16_t meter_adr, uint8_t client_adr)
{
  if ( (meter_adr > 0x3FFF) || (client_adr > 0x7F) )
    return;
  DA_SA_response[3] = (meter_adr >> 7)<<1;
  DA_SA_response[4] = ( (meter_adr & 0x7f)<<1 )|1;

  DA_SA_response[0] = (client_adr << 1)|1;
}
//--------------------------------------------------------------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//static float ERROR = numeric_limits<float>::max();

// TODO в название нужно делать соответствующее назначению функции CalcPackSize например
uint16_t f_HDLC_CalcPackSize(uint8_t* data, uint16_t data_len) { // рассчитывает длину входящего/исходящего сообщения
  uint16_t len = 1;
  uint16_t i = 1;

  if (data[0] == FLAG)
  {
    while ( (data[i] != FLAG) && (i < data_len) ) // TODO в функцию нужно передавать длину данных.Тогда ее можно вставить вместо 1000
    {
      len++;
      i++;
    }
    return len+1; 
  }
  return 0;
}
//---------------------------------------MAC-----------------------------------------
uint8_t f_MAC_unpack(t_cont* control_LLC, uint8_t** data, uint16_t* data_len, uint8_t* pack, uint16_t len) // Размер в байтах
 {
  t_HDLC_packet get_pack;//= { NULL };

  get_pack.begin = (t_HDLC_packet_begin*)pack; 

  if(memcmp(get_pack.begin ->DA_SA, DA_SA_response, sizeof(DA_SA_response)) != 0)
  {
    return 1;//не совпадение адреса приема/передачи
  }

  if (get_pack.begin->flag_open != FLAG)
    {
      return 2; //необнаружен открывающий флаг
    }
    if ((get_pack.begin->control & 1) == 1) {
      if ((get_pack.begin->control & 3) == 3) {
        control_LLC->type = (get_pack.begin->control & U_FORM);
        /*
        switch (get_pack.begin->control & U_FORM)//U-формат
        {
          case UI://короткая информация
            control_LLC.type=UI;
            break;
          case SNRM://режим нормального ответа
            control_LLC.type=SNRM;
            break;
          case DISC://разъединить
            control_LLC.type=DISC;
            break;
          case UP://ненумерованный опрос
            control_LLC.type=UP;
            break;
          case UA://подтверждение
            control_LLC.type=UA;
            break;
          case TEST://проверка
            control_LLC.type=TEST;
            break;
          case SIM://режим инициативного выхода
            break;
          case FRMR://неприем кадра
            break;
          case DM://"Разъединено"
            break;
          case RSET://сброс счетчика принятых кадров
            break;
          case CF://режим длинного асинхронного ответа
            break;
          case SNRME://режим длинного нормального ответа
            break;
          case SABM://асинхронный сбалансированный режим
            break;
          case XID://идентификация станции
            break;
          case SABME://режим длинного сбалансированного асинхронного ответа
            break;
          default:
            break;
         
        }
        */
      }
      else{
        control_LLC->type = (get_pack.begin->control & S_FORM);//S-формат
        /*
        switch (get_pack.begin->control & S_FORM)//S-формат
        {
          case RR:   //готов к приему
            break;
          case RNR:  //не готов к приему
            break;
          case REJ:  //отказ в приеме
            break;
          case SREJ: //выборочный отказ в приеме
          default:
            break;
        }
       */
      }
    }
    else {
       control_LLC->type = I_FORM;//I-формат
    }
    control_LLC->data = get_pack.begin->control;
/*
  else{
    return 2; //необнаружен открывающий флаг
  }
*/
  if (get_pack.begin->HCS != f_crc16(&pack[1], HEADER_LEN))
  {
    return 3; //несовпадение контрольной суммы кадра
  }

  get_pack.end = (t_HDLC_packet_end*)(&pack[f_HDLC_CalcPackSize(pack, len) - 3]);

  t_format form;
  form.point = swap((uint8_t*)&get_pack.begin->format.point, 2);

  if (form.form.size != f_HDLC_CalcPackSize(pack, len) - 2) {
    return 4;//длинна зашифрованная в посылки не совпала с действительной;
  }

  if ( get_pack.end->FCS != f_crc16(&pack[1], (f_HDLC_CalcPackSize(pack, len) - 4)) )
  {
    return 6;//контрольная сумма кадра не совпала
  }

  if (get_pack.end-> flag_close != FLAG)
  {
    return 5;//отсутствует флаг окончания сообщения
  }

  *data = &get_pack.begin->data;
  *data_len = (f_HDLC_CalcPackSize(pack, len) - 6);
return 111;
}
//--------------------------------------------------------------------------------
