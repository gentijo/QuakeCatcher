#include <stdio.h>
#include <util/delay.h>
#include "sensors.h"
#include "ioinit.h"
#include "iotest.h"

#include "../lib/timer128.h"
#include "../network/gswifi/gs.h"
#include "ioinit.h"


int main()
{
  ioinit();

  initSensorTimers();

  // enable interrupts globally
  sei();

  gs_Init(&Serial1, "ProjectGIO", "QuakeCatcher");

  connectionId = gs_open_connection("192.168.0.110", "8125");

  printf("\nInit Complete\n");

//  wifi_test();

  initSensorModule();
  sensorMainLoop();

  return 0;
}
