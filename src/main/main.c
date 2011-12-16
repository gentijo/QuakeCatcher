
#include <avr/io.h>
#include "../lib/Util.h"
#include "../lib/Comm.h"


int main()
{
	CLKPR = 0x80;
	CLKPR = 0x81;

	commInit2(38400, true, true);

	for(int x=0; x<100000; x++)
	{
		commPrintLn("Hello World");
	}

	while(true);
	return 0;
}

