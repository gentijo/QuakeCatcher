#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "../avr/driver/i2c.h"
#include "../device/HMC5843/HMC5843.h"
#include "../device/bma180/bma180_i2c.h"

// All the sensors 2 bytes per axis, 3 axis
#define BYTES_PER_SENSOR_SAMPLE 6

#define SAMPLES_PER_SECOND 50

struct SensorPage {
  uint8_t accel[BYTES_PER_SENSOR_SAMPLE * SAMPLES_PER_SECOND];
  uint8_t mag[]
};
