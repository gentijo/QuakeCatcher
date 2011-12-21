
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "../lib/Util.h"
#include "../lib/Comm.h"
#include "../procyon/i2c.h"
#include "../device/HMC5843/HMC5843.h"

static int uart_putchar (char c, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM (uart_putchar, NULL, _FDEV_SETUP_WRITE);




int main()
{
	CLKPR = 0x80;
	CLKPR = 0x81;

	stdout = &mystdout;

	commInit2(38400, true, true);
  i2cInit();
  _delay_ms(100);
	printf("Quake Catcher Development Board V0.99\n");

  HMC5843Test();

	for(int x=0; x<100000; x++)
	{
		printf("Hello World\n");
		_delay_ms(1000);
	}

	while(true);
	return 0;
}


static int uart_putchar (char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream);
    commTxCh(c);
    return 0;
}
