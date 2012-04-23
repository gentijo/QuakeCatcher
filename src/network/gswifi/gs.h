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
HTTPMesg* gs_send_HTTPMessage(u08 *URL, HTTP_MesgType type, HTTPMesg *mesg);
int       gs_get_NTPTime();


#endif /* GAINSPAN_H_ */
