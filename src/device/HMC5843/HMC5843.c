
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include "../../avr/driver/i2c.h"
#include "HMC5843.h"

///============Function Prototypes=========/////////////////
void HMC5843(void);
void HMC5843_start_continious(void);
void HMC5843_start_single(void);

/////===================================////////////////////
uint8_t cmd_continious[] = {0x02, 0x00};
uint8_t cmd_single[] = {0x02, 0x01};


int HMC5843Test(void)
{
  HMC5843_start_single();
  _delay_ms(100); //at least 100ms interval between measurements

	while(1)
	{
		HMC5843();
		_delay_ms(400); //at least 100ms interval between measurements
	}
}

void HMC5843(void)
{		
	uint8_t data[6];

  printf("\n");

	i2cMasterReceiveNI(0x3c, sizeof(data), data);
  printf("\n");

  long v;
  v = (data[0] << 8) | data[1];
  printf("x=%4ld, ", v);


  v = (data[2] << 8) | data[3];
  printf("y=%4ld, ", v );

  v = (data[4] << 8) | data[5];
  printf("z=%4ld\n ",v );

  HMC5843_start_single();
}


void HMC5843_start_single()
{
  i2cMasterSendNI(0x3c, sizeof(cmd_continious), cmd_single);
}

void HMC5843_start_continious(void)
{
	i2cMasterSendNI(0x3c, sizeof(cmd_continious), cmd_continious);
}
