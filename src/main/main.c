#include <stdio.h>
#include <util/delay.h>
#include "sensors.h"
#include "ioinit.h"
#include "iotest.h"

#include "../lib/timer128.h"


int main()
{
  ioinit();

  initSensorTimers();

  // enable interrupts globally
  sei();

  gs_Init(&Serial1);

  connectionId = gs_open_connection("at+NCTCP=192.168.1.238,8125");

  printf("\nInit Complete\n");

//  wifi_test();

  initSensorModule();
  sensorMainLoop();

  return 0;
}


