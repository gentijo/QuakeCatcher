
#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>

#include <util/delay.h>
#include "global.h"
#include "../../avr/driver/i2c.h"

#include "bma180_i2c.h"

bool ee_w = false;

static    u08     m_valueRegAddr = 0x02;
volatile  bool    sensorDataReady = false;
static    u08     *sensorData;

void bma180_ReadSensorData_p2();
void bma180_ReadSensorData_p3();

static genericVoidCallbackFn sampleCompleteCallback;

void bma180_test()
{
  u08 sensorDataBuffer[6];

  bma180_init();
  bma180_SetRange(r2G);
  bma180_SetBandwidth(bw40Hz);

  while(1)
  {
      bma180_ReadSensorData(sensorDataBuffer, NULL);
      while(!sensorDataReady);

      uint16_t x, y, z;
      x = sensorDataBuffer[1]<<8|sensorDataBuffer[0];
      x = x >> 2;

      y = sensorDataBuffer[3]<<8|sensorDataBuffer[2];
      y = y >> 2;

      z = sensorDataBuffer[5]<<8|sensorDataBuffer[4];
      z= z >> 2;
      
      printf("X=%d, Y=%d, Z=%d\n", x, y, z);
      _delay_ms(500);
  }


}

void bma180_init()
{
  // Reset the device to get it to "known" state
  bma180_WriteReg(0x10, 0xB6);
  _delay_ms(20);
}

// Shorthand helpers for writing various registers
void bma180_set_ee_w(bool enable)
{
  if (enable) bma180_WriteReg(0x0d, 0B00010000);
  else bma180_WriteReg(0x0d, 0B00000000);
  ee_w = enable;
}

bool bma180_SetRange(enum Range range)
{
  switch (range)
  {
    case r1G: // 1g
    case r1_5G: // 1.5g
    case r2G: // 2g
    case r3G: // 3g
    case r4G: // 4g
    case r8G: // 8g
    case r16G: // 16g
      if (!ee_w)
      {
        bma180_set_ee_w(true);
      }
      return bma180_ModReg(0x35, 0B11110001, ((uint8_t)range) << 1);
      break;

    default:
      printf("BMA180 - Invalid value range %d", range);
      return false;
  }
}

bool bma180_SetBandwidth(enum Bandwidth bw)
{
  switch (bw)
  {
      case bw10Hz:
      case bw20Hz:
      case bw40Hz:
      case bw75Hz:
      case bw150Hz:
      case bw300Hz:
      case bw600Hz:
      case bw1200Hz:
      case bwHighPass:
      case bwBandPass:
      
      if (!ee_w)
      {
        bma180_set_ee_w(true);
      }
      
      return bma180_ModReg(0x20, 0B00001111, ((uint8_t) bw) << 4);
      _delay_ms(10);
      break;
      
    default:
      printf("BMA-180 Invalid bandwidth %d", bw);
      return false;
  }
}


bool bma180_SetNewDataInterrupt(bool enable)
{
  if (!ee_w)
  {
      bma180_set_ee_w(true);
  }
  return bma180_ModReg(0x21, 0B11111101, (enable & 0B00000001) << 1);
}

bool bma180_Set_SMP_Skip(bool enable)
{
  if (!ee_w)
  {
       bma180_set_ee_w(true);
  }
  return bma180_ModReg(0x35, 0B1111110, (enable & 0B00000001));
}

void bma180_ReadSensorData(u08 *buffer, genericVoidCallbackFn callback)
{
  sensorData = buffer;
  sampleCompleteCallback = callback;
  sensorDataReady=false;
  i2cMasterSend(BMA180_DeviceID, 1, &m_valueRegAddr, bma180_ReadSensorData_p2);
}

void bma180_ReadSensorData_p2()
{
  // printf("bma180-p2\n");
  i2cMasterReceive(BMA180_DeviceID, 6, sensorData, bma180_ReadSensorData_p3);
}

void bma180_ReadSensorData_p3()
{
  // printf("bma180-p3\n");
  sensorDataReady=true;
  if (sampleCompleteCallback != NULL) sampleCompleteCallback();
}

bool bma180_ReadReg(uint8_t address, uint8_t *value)
{
  if (i2cMasterSendNI(BMA180_DeviceID, 1, &address) != I2C_OK) return false;
  if (i2cMasterReceiveNI(BMA180_DeviceID, 1, value) != I2C_OK) return false;
  return true;
}


uint8_t reg_write_buf[2];
bool bma180_WriteReg(uint8_t address, uint8_t value)
{
  reg_write_buf[0] = address;
  reg_write_buf[1] = value;
  if (i2cMasterSendNI(BMA180_DeviceID, sizeof(reg_write_buf), reg_write_buf) != I2C_OK) return false;
  return true;
}

bool bma180_ModReg(uint8_t address, uint8_t mask, uint8_t value)
{
  uint8_t tmp;
  if (!bma180_ReadReg(address, &tmp))
  {
    return false;
  }
  tmp = (tmp & mask) | value;
  return bma180_WriteReg(address, tmp);
}




