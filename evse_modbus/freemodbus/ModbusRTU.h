#ifndef _DEMO_H
#define _DEMO_H

#include <stdbool.h>
#include <stdint.h>
#include "port.h"
#include "mbport.h"

#ifdef __cplusplus
  extern "C" {
#endif  /* __cplusplus */

// ----------------------------------------------------------------------------
typedef struct
{
  uint16_t *p_array;
  uint16_t size;
  uint16_t first_reg_num;

} modbus_registers_array_t;
// ----------------------------------------------------------------------------
typedef struct
{
  bool (*inputsReadCb)(uint16_t *p_data, uint16_t addr, uint16_t reg_count);
  bool (*holdingReadCb)(uint16_t *p_data, uint16_t addr, uint16_t reg_count);
  bool (*coilWriteCb)(uint16_t addr, uint16_t value);
  bool (*holdingWriteCb)(uint16_t addr, uint16_t value);

} registers_access_callbacks_t;

// ----------------------------------------------------------------------------
typedef struct
{

  void (*pSerialEnableCallback)( bool xRxEnable, bool xTxEnable );
  bool (*pSerialInitCallback)( void );
  bool (*pSerialPutByteCallback)( int8_t ucByte );
  bool (*pSerialGetByteCallback)( int8_t * pucByte );

} serial_callbacks_t;
// ----------------------------------------------------------------------------
typedef struct
{

  bool (*pTimersInitCallback)( uint16_t usTim1Timerout50us );
  void (*pTimersEnableCallback)( void );
  void (*pTimersDisableCallback)( void );

} timers_callbacks_t;
// ----------------------------------------------------------------------------
typedef struct
{
  uint8_t mb_addr;

  registers_access_callbacks_t regs_write_callbacks;

  serial_callbacks_t serial;
  timers_callbacks_t timer;

} init_modbus_struct_t;
// ----------------------------------------------------------------------------

void prvvUARTTxReadyISR( void );
void prvvUARTRxISR( void );

void prvvTIMERExpiredISR( void );

int ModbusRtuInit( init_modbus_struct_t *init );

int ModbusRun( void );

#ifdef __cplusplus
  }
#endif  /* __cplusplus */


#endif /* _DEMO_H */

