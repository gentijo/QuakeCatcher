/*
 * GainSpan.h
 *
 *  Created on: Apr 18, 2012
 *      Author: gentijo
 */

#ifndef GAINSPAN_H_
#define GAINSPAN_H_

#include <stdio.h>
#include <stdlib.h>
#include "lib/time.h"
#include "global.h"

typedef struct MesgBuf {
  u08 *buf;
  u16 capicity;
  u16 len;
} MesgBuf;

typedef struct HTTPMesg {
  MesgBuf txbuf;
  MesgBuf rxbuf;
  u08 resultCode;
  u08 connectionID;
} HTTPMesg;


typedef enum HTTP_MesgType {
  HTTP_POST_MESG=1,
  HTTP_GET_MESG=2
} HTTP_MesgType;

void      gs_Init(FILE *port);

int       gs_open_connection(char *address);
bool      gs_send_data(int connid, MesgBuf *txdata, MesgBuf *rxdata);
bool      gs_close_connection(int cid);

time_t    gs_get_NTPTime();

bool gs_start_data(u16 length, short connectionId);
FILE *gs_get_modem_port();



#endif /* GAINSPAN_H_ */
