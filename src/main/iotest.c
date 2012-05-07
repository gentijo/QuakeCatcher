#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "string.h"

#include "../lib/Util.h"
#include "../lib/Comm.h"
#include "../avr/driver/i2c.h"
#include "../device/HMC5843/HMC5843.h"
#include "../device/bma180/bma180_i2c.h"
#include "../avr/driver/uart2.h"
#include "../avr/rprintf.h"
#include "sensors.h"
#include "network/gswifi/gs.h"

int wifi_test()
{
  gs_Init(&Serial1, "", "");
  gs_get_NTPTime();

  u08 sampleData[100] = "Hello World";
  MesgBuf buf= {
      sampleData,
      100,
      0
  };
  buf.len=strlen(sampleData);


  int connid;
  for (int x=0; x< 3; x++)
  {
    connid = gs_open_connection("192.168.1.250", "8125");
    if (connid >= 0) break;
  }

  if (connid >= 0)
  {
    gs_send_data(connid, &buf, NULL );
   // gs_close_connection(connid);
  }


  uartLoopBack();

  return 0;
}

/**
 * Full-duplex send/receive between UART0 and UART1
 */
void uartLoopBack()
{
  printf("UART Serial IO Tester\n");

  while (true) {
    sendIO(0, 1);
    sendIO(1, 0);
  }
}

void sendIO(u08 uartReceiveId, u08 uartSendId)
{
	u08 data;
	bool dataToSend = false;

	while (uartReceiveByte(uartReceiveId, &data))
	{
		dataToSend = true;
		uartAddToTxBuffer(uartSendId, data);
	}

	if (dataToSend)
		uartSendTxBuffer(uartSendId);
}


