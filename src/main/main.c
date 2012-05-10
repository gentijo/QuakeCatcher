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

  gs_Init(&Serial1, "ecs-office-net", "");

  // connectionId = gs_open_connection("192.168.100.249", "8125");
  connectionId = gs_open_connection("192.168.1.237", "8125");

  printf("\nInit Complete\n");

//  wifi_test();

  initSensorModule();
  sensorMainLoop();

  return 0;
}
