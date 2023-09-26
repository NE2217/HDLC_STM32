#include <stdlib.h>
#include <stdint.h>
#include "MAC_Pack.h"
#include "CRC.h"

uint8_t f_HDLC_pack(uint16_t* len_pack, uint8_t* buf, uint16_t len, uint8_t* data, uint16_t len_data, uint8_t* DA_SA, uint8_t control)//Размер в байтах
{  
  uint16_t* pack_size = len_pack;
  *pack_size = len_data + CONTROL_FIELDS_SIZE;
  if (*pack_size > len)
    return 1;//предоставленно недостаточно места
  t_HDLC_packet p_pack;
  p_pack.begin = (t_HDLC_packet_begin*)buf;
  p_pack.end = (t_HDLC_packet_end*) (&buf[ *pack_size - sizeof(t_HDLC_packet_end) ]);

  p_pack.begin->flag_open = FLAG;

  p_pack.begin->format.form.typ = FORMAT_TYPE;
  p_pack.begin->format.form.S = FORMAT_S;
  p_pack.begin->format.form.size = len_data + CONTROL_FIELDS_SIZE-2;
  p_pack.begin->format.point = swap((uint8_t*)&p_pack.begin->format.point, 2);

  memcpy(p_pack.begin->DA_SA, DA_SA, sizeof(p_pack.begin->DA_SA));
  p_pack.begin->control = control;
  p_pack.begin->HCS = f_crc16( ( (uint8_t*) &p_pack.begin->format.point ), HEADER_LEN );
  memcpy((uint8_t*)&p_pack.begin->data, data, len_data);
  p_pack.end->FCS = f_crc16(((uint8_t*)&p_pack.begin->format.point), *pack_size - 2);
  p_pack.end->flag_close = FLAG;

  return 0;
}