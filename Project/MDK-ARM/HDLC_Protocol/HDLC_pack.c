#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "HDLC_Protocol.h"
#include "HDLC_pack.h"
#include "MAC_Pack.h"
#include "CRC.h"
//--------------------------------------------------------------------------------
#define FLAG                   0x7e
#define FCS_SIZE                  2
#define CONTROL_FIELDS_SIZE      14
#define LEN_DATA_REQUEST         16
#define LOCAL_BUF_LEN           100
#define DEFAULT_PASWOR   "12345670"
//--------------------------------------------------------------------------------
#pragma pack(push,1)
typedef enum {
  SEND_CURRENT_A=0,
  SEND_CURRENT_B,
  SEND_CURRENT_C,
  SEND_VOLT_A,
  SEND_VOLT_B,
  SEND_VOLT_C,
  SEND_SUM_Q,
  SEND_SUM_P,
  SEND_SUM_S,
  SEND_COS_F,
  SEND_ACTIV_ENERGE_IMPORT,
  SEND_REACTIV_ENERGE_IMPORT
}t_send_stat_param;
typedef union {
  uint8_t point;
  struct
  {
    unsigned I: 1;
    unsigned NR:3;
    unsigned PS:1;
    unsigned NS:3;
  }Is;
} t_control_comand;
/*
typedef struct {
  uint8_t A;
  uint8_t B;
}t_AB_test;
*/
typedef struct {
  uint8_t start[33];
  uint8_t password[8];
  uint8_t end[18];
}t_pac_password;

#pragma pack(pop)
//--------------------------------------------------------------------------------
// OBIS коды запрашиваемых данных
static const uint8_t Send_current_A[]              = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x1F,0x07,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_current_B[]              = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x33,0x07,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_current_C[]              = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x47,0x07,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_volt_A[]                 = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x20,0x07,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_volt_B[]                 = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x34,0x07,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_volt_C[]                 = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x48,0x07,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_sum_Q[]                  = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x01,0x07,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_sum_P[]                  = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x03,0x07,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_sum_S[]                  = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x09,0x07,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_cos_f[]                  = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x0D,0x07,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_activ_energy_import[]    = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x01,0x08,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_reactiv_energy_import[]  = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x03,0x08,0x00,0xFF,0x02, 0x00};
static const uint8_t Send_apparent_energy_import[] = {0xE6,0xE6,0x00,0xC0,0x01, 0x40,0x00,0x03,0x01,0x00, 0x09,0x08,0x00,0xFF,0x02, 0x00};

static const uint8_t* Send_comands_points[] = {Send_current_A, Send_current_B, Send_current_C, Send_volt_A, Send_volt_B,
                                               Send_volt_C, Send_sum_Q, Send_sum_P, Send_sum_S, Send_cos_f, Send_activ_energy_import,
                                               Send_reactiv_energy_import, Send_apparent_energy_import};
uint8_t PosParam=0;
// TODO задефайнить адреса
uint8_t DA_SA_request[] = { 0x00, 0x02, 0x44, 0xC9, 0x41 };

  uint8_t request_connect_LLC[] = { 0x81, 0x80, 0x14, 0x05, 0x02,
  0x04, 0x00, 0x06, 0x02, 0x04, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
  0x00, 0x01 };

  uint8_t request_authorize[] = { 0x7E, 0xA0, 0x47, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x10, 0x17, 0x55, 0xE6, 0xE6, 0x00, 0x60, 0x36,
  0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0x8A, 0x02, 0x07, 0x80, 0x8B,
  0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x02, 0x01, 0xAC, 0x0A, 0x80, 0x08, 
  0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
  0xBE, 0x10, 0x04, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0xFF, 0xFF, 0xFF, 0x08, 0x00, 0xD5, 0x24, 0x7E };

  uint8_t request_authorize_start[] = { 0x7E, 0xA0, 0x47, 0x00, 0x02, 0x44, 0xC9, 0x41, 0x10, 0x17, 0x55, 0xE6, 0xE6, 0x00, 0x60, 0x36,
  0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0x8A, 0x02, 0x07, 0x80, 0x8B,
  0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x02, 0x01, 0xAC, 0x0A, 0x80, 0x08 };
  uint8_t request_authorize_password[] = DEFAULT_PASWOR;
  uint8_t request_authorize_end[] = { 0xBE, 0x10, 0x04, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x06,
  0x5F, 0x1F, 0x04, 0x00, 0xFF, 0xFF, 0xFF, 0x08, 0x00, 0xD5, 0x24, 0x7E };

  uint8_t request_authorize_start_LLC[] = { 0xE6, 0xE6, 0x00, 0x60, 0x36,
  0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0x8A, 0x02, 0x07, 0x80, 0x8B,
  0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x02, 0x01, 0xAC, 0x0A, 0x80, 0x08 };
  uint8_t request_authorize_password_LLC[] = DEFAULT_PASWOR;
  uint8_t request_authorize_end_LLC[] = { 0xBE, 0x10, 0x04, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x06,
  0x5F, 0x1F, 0x04, 0x00, 0xFF, 0xFF, 0xFF, 0x08, 0x00 };

uint8_t LocalBuf[LOCAL_BUF_LEN] = {0};

//--------------------------------------------------------------------------------

uint8_t HDLC_PackSendConfigParam(void (*uartSendDataCB)(uint8_t *data, uint16_t len))
{
  uint16_t pack_size = sizeof(request_connect_LLC) + CONTROL_FIELDS_SIZE;
  t_HDLC_packet p_pack;

  p_pack.begin = (t_HDLC_packet_begin*)LocalBuf;
  p_pack.end = (t_HDLC_packet_end*) (&LocalBuf[ pack_size - sizeof(t_HDLC_packet_end) ]);

  p_pack.begin->flag_open = FLAG;

  p_pack.begin->format.form.typ  = FORMAT_TYPE;
  p_pack.begin->format.form.S    = FORMAT_S;
  p_pack.begin->format.form.size = pack_size-2;
  p_pack.begin->format.point     = swap((uint8_t*)&p_pack.begin->format.point, 2);

  memcpy(p_pack.begin->DA_SA, DA_SA_request, sizeof(DA_SA_request));
  p_pack.begin->control  = SNRM|(1 << 4);
  p_pack.begin->HCS      = f_crc16( ( (uint8_t*) &p_pack.begin->format.point ), HEADER_LEN );

  memcpy((uint8_t*)&p_pack.begin->data, request_connect_LLC, sizeof(request_connect_LLC));
  p_pack.end->FCS        = f_crc16(((uint8_t*)&p_pack.begin->format.point), pack_size - 4);
  p_pack.end->flag_close = FLAG;

  uartSendDataCB(LocalBuf, pack_size);
  
  //uartSendDataCB(request_connect, sizeof(request_connect));
// TODO  TimeOutReset();
// TODO  WaitingForResponse = true;
  return 0;
}

uint8_t HDLC_PackSendAuthorization(void (*uartSendDataCB)(uint8_t *data, uint16_t len), uint8_t N)
{
  t_pac_password request_authorize_p;

  memcpy(request_authorize_p.start, request_authorize_start_LLC, sizeof(request_authorize_start_LLC));
  memcpy(request_authorize_p.password, request_authorize_password_LLC, sizeof(request_authorize_password_LLC));
  memcpy(request_authorize_p.end, request_authorize_end_LLC, sizeof(request_authorize_end_LLC));
//---
  t_control_comand control;
  control.Is.NR = N;
  control.Is.PS = 1;
  control.Is.NS = N;
  control.Is.I =  0;
  
  uint16_t pack_size = sizeof(request_authorize_p)/*59*/ + CONTROL_FIELDS_SIZE;
  t_HDLC_packet p_pack;

  p_pack.begin = (t_HDLC_packet_begin*)LocalBuf;
  p_pack.end = (t_HDLC_packet_end*) (&LocalBuf[ pack_size - sizeof(t_HDLC_packet_end) ]);

  p_pack.begin->flag_open = FLAG;

  p_pack.begin->format.form.typ  = FORMAT_TYPE;
  p_pack.begin->format.form.S    = FORMAT_S;
  p_pack.begin->format.form.size = pack_size-2;
  p_pack.begin->format.point     = swap((uint8_t*)&p_pack.begin->format.point, 2);

  memcpy(p_pack.begin->DA_SA, DA_SA_request, sizeof(DA_SA_request));
  p_pack.begin->control  = control.point;
  p_pack.begin->HCS      = f_crc16( ( (uint8_t*) &p_pack.begin->format.point ), HEADER_LEN );

  memcpy((uint8_t*)&p_pack.begin->data, request_authorize_p.start, sizeof(request_authorize_p));
  p_pack.end->FCS        = f_crc16(((uint8_t*)&p_pack.begin->format.point), pack_size - 4);
  p_pack.end->flag_close = FLAG;
//---
  uartSendDataCB(LocalBuf, pack_size);

  return 0;
}
//--------------------------------------------------------------------------------

uint8_t HDLC_PackSendComand(uint8_t N,void (*uartSendDataCB)(uint8_t *data, uint16_t len), uint8_t PosParam)
{
  uint16_t pack_size = LEN_DATA_REQUEST + CONTROL_FIELDS_SIZE;
  t_control_comand control;
  control.Is.NR = N;
  control.Is.PS = 1;
  control.Is.NS = N;
  control.Is.I =  0;

/*
  if (*pack_size > len)
    return 1;//предоставленно недостаточно места
*/
  t_HDLC_packet p_pack;

  p_pack.begin = (t_HDLC_packet_begin*)LocalBuf;
  p_pack.end = (t_HDLC_packet_end*) (&LocalBuf[ pack_size - sizeof(t_HDLC_packet_end) ]);

  p_pack.begin->flag_open = FLAG;

  p_pack.begin->format.form.typ  = FORMAT_TYPE;
  p_pack.begin->format.form.S    = FORMAT_S;
  p_pack.begin->format.form.size = pack_size-2;
  p_pack.begin->format.point     = swap((uint8_t*)&p_pack.begin->format.point, 2);

  memcpy(p_pack.begin->DA_SA, DA_SA_request, sizeof(DA_SA_request));
  p_pack.begin->control  = control.point;
  p_pack.begin->HCS      = f_crc16( ( (uint8_t*) &p_pack.begin->format.point ), HEADER_LEN );

  memcpy((uint8_t*)&p_pack.begin->data, Send_comands_points[PosParam], LEN_DATA_REQUEST);
  p_pack.end->FCS        = f_crc16(((uint8_t*)&p_pack.begin->format.point), pack_size - 4);
  p_pack.end->flag_close = FLAG;

  uartSendDataCB(LocalBuf, pack_size);

  return 0;
}
//----------------------------------------------------------------
uint8_t HDLC_PackDisconnect (void (*uartSendDataCB)(uint8_t *data, uint16_t len))
{
  uint16_t pack_size = CONTROL_FIELDS_SIZE - FCS_SIZE;

  t_HDLC_packet p_pack;

  p_pack.begin = (t_HDLC_packet_begin*)LocalBuf;
  p_pack.end = (t_HDLC_packet_end*) (&LocalBuf[ pack_size - sizeof(t_HDLC_packet_end) ]);

  p_pack.begin->flag_open = FLAG;

  p_pack.begin->format.form.typ  = FORMAT_TYPE;
  p_pack.begin->format.form.S    = FORMAT_S;
  p_pack.begin->format.form.size = pack_size - 2;
  p_pack.begin->format.point     = swap((uint8_t*)&p_pack.begin->format.point, 2);

  memcpy(p_pack.begin->DA_SA, DA_SA_request, sizeof(DA_SA_request));
  p_pack.begin->control  = DISC|(1<<4);
  p_pack.begin->HCS      = f_crc16( ( (uint8_t*) &p_pack.begin->format.point ), HEADER_LEN );

  p_pack.begin->data = FLAG;

  uartSendDataCB(LocalBuf, pack_size);

  return 0;
}
//----------------------------------------------------------------
void HDLC_PackAdr(uint16_t meter_adr, uint8_t client_adr)
{
  if ( (meter_adr > 0x3FFF) || (client_adr > 0x7F) )
    return;
  DA_SA_request[2] = (meter_adr >> 7)<<1;
  DA_SA_request[3] = ( (meter_adr & 0x7f)<<1 )|1;
  
  DA_SA_request[4] = (client_adr << 1)|1;
}
