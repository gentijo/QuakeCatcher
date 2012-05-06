/*
 * init.c
 *
 *  Created on: May 6, 2012
 *      Author: gentijo
 */
#include "ioinit.h"

static int uart0_putchar (char c, FILE *stream);
static int uart0_getchar (FILE *stream);
static int uart1_putchar (char c, FILE *stream);
static int uart1_getchar (FILE *stream);

FILE Serial0 = FDEV_SETUP_STREAM (uart0_putchar, uart0_getchar, _FDEV_SETUP_RW);
FILE Serial1 = FDEV_SETUP_STREAM (uart1_putchar, uart1_getchar, _FDEV_SETUP_RW);



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

  uart0Init();
  uart1Init();
  stdout = &Serial0;

  _delay_ms(100);

}

static int uart0_putchar (char c, FILE *stream)
{
    if (c == '\n')
        uart0_putchar('\r', stream);

    uartAddToTxBuffer(0, c);
    uartSendTxBuffer(0);

    return 0;
}

static int uart0_getchar (FILE *stream)
{
  u08 data;

  if (uartReceiveByte(0, &data))
  {
      return (int) data;
  }

  return EOF;
}

static int uart1_putchar (char c, FILE *stream)
{
    if (c == '\n')
        uart1_putchar('\r', stream);

    uartAddToTxBuffer(1, c);

    uartSendTxBuffer(1);
    return 0;
}

static int uart1_getchar (FILE *stream)
{
  u08 data;

  if (uartReceiveByte(1, &data))
  {
    return (int) data;
  }
  return EOF;
}
