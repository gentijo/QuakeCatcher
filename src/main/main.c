#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "../lib/Util.h"
#include "../lib/Comm.h"
#include "../avr/driver/i2c.h"
#include "../device/HMC5843/HMC5843.h"
#include "../device/bma180/bma180_i2c.h"
#include "../avr/driver/uart2.h"
#include "../avr/rprintf.h"
#include "sensors.h"
#include "iotest.h"

#include "../lib/timer128.h"
#include "../network/gswifi/gs.h"
#include "ioinit.h"


int main()
{
  ioinit();
  uart0Init();
  uart1Init();
  stdout = &Serial0;

  initSensorTimers();

  // enable interrupts globally
  sei();

  gs_Init(&Serial1, "ecs-office-net", "");

  // connectionId = gs_open_connection("192.168.100.249", "8125");
  connectionId = gs_open_connection("192.168.1.237", "8125");

  printf("\nInit Complete\n");

  initSensorModule();
  sensorMainLoop();

  return 0;
}
