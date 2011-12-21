#include <avr/io.h>
#include <stdio.h>
#include "Util.h"
#include "Comm.h"


static bool comm_Init = false;


void commInit2(int baud, bool fRx, bool fTx)
{
	comm_Init = 1;

	if (fRx)
		SetBit(UCSR0B, RXEN0);	// enable receive
	else
		ClearBit(UCSR0B, RXEN0);	// disable receive

	if (fTx)
		SetBit(UCSR0B, TXEN0);	// enable transmit
	else
		ClearBit(UCSR0B, TXEN0);	// disable transmit

	int MYUBRR = 0x09;
  UBRR0H = (MYUBRR) >> 8;
  UBRR0L = MYUBRR;

  UCSR0C = (3<<UCSZ00); // Asynchronous 8 N 1
}

/* transmits text via serial connection */
void commPrint(const char *psz)
{
	const char *pch;
	for (pch = psz; *pch != 0; ++pch)
		commTxCh(*pch);
}

void commPrintLn(const char *psz)
{
	if (psz != 0)
		commPrint(psz);
	commPrint("\r\n");
}
	
/* helper function: transmits a single character (after waiting
   for any prev transmission to complete) */
void commTxCh(char ch)
{
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = ch;
}

/* returns true if there is unread data in the receive buffer */
bool commIsCharWaiting()
{
  return( (UCSR0A & (1<<RXC0)) );
}

/* returns the (next) character in the receive buffer (0 if none) */
char commInKey()
{
  if ( (UCSR0A & (1<<RXC0)) ) return UDR0;
	return 0;
}

