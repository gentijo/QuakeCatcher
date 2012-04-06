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

#include "../lib/timer128.h"

static int uart_putchar (char c, FILE *stream);
void ioinit (void);
void uartLoopBack();

static FILE mystdout = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main()
{
  ioinit();
  uart0Init();
  uart1Init();

  stdout = &mystdout;

  initSensorTimers();

  // enable interrupts globally
  sei();

  printf("\nInit Complete\n");

  initSensorModule();
  sensorMainLoop();

  // uartLoopBack();

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



void ioinit (void)
{

	// uartInit();

  CLKPR = 0x80;
  CLKPR = 0x81;

  // Set up Data Direction Register
  // 1 = output, 0 = input
  DDRA = 0;
  DDRB = 0; //PORTB4, B5 output
  DDRD = 0; //PORTD (RX on PD0), PD2 is status output

  // TWI is PC0 & PC1
  DDRC = 0; // All Inputs
  PORTC = 0b00000011; //pullups on the I2C bus

  // PRTWI: Power Reduction TWI
  // PRTIM2: Power Reduction Timer/Counter2
  // PRTIM0: Power Reduction Timer/Counter0
  // PRUSART1: Power Reduction USART1
  // PRTIM1: Power Reduction Timer/Counter1
  // PRSPI: Power Reduction Serial Peripheral Interface
  // PRUSART0: Power Reduction USART0
  // PRADC: Power Reduction ADC
  // PRR = (1<<PRTIM2) | (1<<PRTIM0) | (1<<PRUSART1) | (1<<PRTIM1) | (1<<PRSPI) | (1<<PRADC);

  i2cInit();

  _delay_ms(100);

}

static int uart_putchar (char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream);

    uartAddToTxBuffer(0, c);
    uartSendTxBuffer(0);

    return 0;
}
