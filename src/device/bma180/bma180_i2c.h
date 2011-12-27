/*
 * bma180_i2c.h
 *
 *  Created on: Dec 26, 2011
 *      Author: gentijo
 */

#ifndef BMA180_I2C_H_
#define BMA180_I2C_H_

#include <stdio.h>
#include <stdbool.h>

#define BMA180_DeviceID 0x80
//#define BMA180_DeviceID 0x42


enum Range {

  r1G =   0,
  r1_5G = 1,
  r2G =   2,
  r3G =   3,
  r4G =   4,
  r8G =   5,
  r16G =  6
};

enum Bandwidth {
  bw10Hz = 0,
  bw20Hz = 1,
  bw40Hz = 2,
  bw75Hz = 3,
  bw150Hz = 4,
  bw300Hz = 5,
  bw600Hz = 6,
  bw1200Hz = 7,
  bwHighPass = 8,
  bwBandPass = 9
};

void bma180_test();

void bma180_init();
void bma180_set_ee_w(bool enable);
bool bma180_SetRange(enum Range range);
bool bma180_SetBandwidth(enum Bandwidth bw);

bool bma180_SetNewDataInterrupt(bool enable);
bool bma180_Set_SMP_Skip(bool enable);

bool bma180_ReadSensorData(uint8_t *value);
bool bma180_ReadReg(uint8_t address, uint8_t *target);
bool bma180_WriteReg(uint8_t address, uint8_t value);
bool bma180_ModReg(uint8_t address, uint8_t mask, uint8_t value);


#endif /* BMA180_I2C_H_ */
