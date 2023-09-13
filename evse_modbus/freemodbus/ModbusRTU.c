/*
 * FreeModbus Libary: BARE Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include <stddef.h>
#include <string.h>
#include "ModbusRTU.h"
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/

//static data_arrays_t MbRegisters;
static registers_access_callbacks_t MbRegsWriteCallbacks;

/* ----------------------- Start implementation -----------------------------*/
static eMBErrorCode eStatus;

int ModbusRtuInit( init_modbus_struct_t *init )
{
  if(init == NULL)
    return -1;

  xMBPortSerialInitCallback    = init->serial.pSerialInitCallback;
  vMBPortSerialEnableCallback  = init->serial.pSerialEnableCallback;
  xMBPortSerialPutByteCallback = init->serial.pSerialPutByteCallback;
  xMBPortSerialGetByteCallback = init->serial.pSerialGetByteCallback;

  xMBPortTimersInitCallback    = init->timer.pTimersInitCallback;
  vMBPortTimersEnableCallback  = init->timer.pTimersEnableCallback;
  vMBPortTimersDisableCallback = init->timer.pTimersDisableCallback;

  memcpy(&MbRegsWriteCallbacks, &init->regs_write_callbacks, 
                          sizeof(registers_access_callbacks_t) );

  eStatus = eMBInit( MB_RTU, init->mb_addr, 0, 38400, MB_PAR_EVEN );
  /* Enable the Modbus Protocol Stack. */
  eStatus = eMBEnable(  );
  
  return 0;
}
/* ----------------------- Start implementation -----------------------------*/
int ModbusRun( void )
{
  ( void )eMBPoll(  );
  return 0;
}
// ----------------------------------------------------------------------------
eMBErrorCode RegisterWriteRequestHandler( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                                      USHORT *usRegsBuffer, USHORT usRegStart, USHORT usRegsAmount)
{
  eMBErrorCode eStatus = MB_ENOERR;
  int          iRegIndex;

  if( ( usAddress >= usRegStart )
    && ( usAddress + usNRegs <= usRegStart + usRegsAmount ) )
  {
    iRegIndex = ( int )( usAddress - usRegStart );
    while( usNRegs > 0 )
    {
      usRegsBuffer[iRegIndex] = 0xFF00 & (*pucRegBuffer++<<8);
      usRegsBuffer[iRegIndex] |= *pucRegBuffer++;

      iRegIndex++;
      usNRegs--;
    }
  }
  else
  {
    eStatus = MB_ENOREG;
  }

  return eStatus;
}
// ----------------------------------------------------------------------------
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  USHORT *p_data;

  if(MbRegsWriteCallbacks.inputsReadCb == NULL)
    return MB_EIO;

  if(MbRegsWriteCallbacks.inputsReadCb((uint16_t*)pucRegBuffer, usAddress, usNRegs))
  {
    p_data = (uint16_t*)pucRegBuffer;
    while(usNRegs-- > 0)
    {
      *p_data = ( (*p_data>>8) & 0x00FF ) | ((*p_data<<8) & 0xFF00); // swap
      ++p_data;
    }

    return MB_ENOERR;
  }

  return MB_EIO;
}
// ----------------------------------------------------------------------------
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  USHORT *p_data;

  if(eMode == MB_REG_READ)
  {
    if(MbRegsWriteCallbacks.holdingReadCb == NULL)
      return MB_EIO;

    if(MbRegsWriteCallbacks.holdingReadCb((uint16_t*)pucRegBuffer, usAddress, usNRegs))
    {
      p_data = (uint16_t*)pucRegBuffer;
      while(usNRegs-- > 0)
      {
        *p_data = ( (*p_data>>8) & 0x00FF ) | ((*p_data<<8) & 0xFF00); // swap
        ++p_data;
      }
      
      return MB_ENOERR;
    }
  }
  if(eMode == MB_REG_WRITE)
  {
    if(MbRegsWriteCallbacks.holdingWriteCb == NULL)
      return MB_EIO;

    if(usNRegs > 1)
      return MB_EIO;

    if(MbRegsWriteCallbacks.holdingWriteCb(usAddress, *pucRegBuffer) == false)
      return MB_EIO;

    return MB_ENOERR;
  }

  return MB_EIO;
}
// ----------------------------------------------------------------------------
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
  if(eMode == MB_REG_READ)
  {
    return MB_ENOREG;
  }
  if(eMode == MB_REG_WRITE)
  {
    if(MbRegsWriteCallbacks.coilWriteCb == NULL)
      return MB_EIO;

    if(MbRegsWriteCallbacks.coilWriteCb(usAddress, *pucRegBuffer) == false)
      return MB_EIO;

    return MB_ENOERR;
  }

  return MB_EIO;
}
// ----------------------------------------------------------------------------
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  return MB_EIO;
}
// ----------------------------------------------------------------------------
