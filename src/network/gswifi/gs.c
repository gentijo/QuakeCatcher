/*
 * GainSpan.c
 *
 *  Created on: Apr 18, 2012
 *      Author: gentijo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>

#include "gs.h"

FILE *modemPort = NULL;

// Basic Response Buffer for AT Commands
char    def_resp_buf[100];
MesgBuf def_response = {
  def_resp_buf, // buf
  100, // capicity;
   0 // u16 len;
};


bool sendATCommand(char *cmd, MesgBuf *response, bool checkOK, u08 waitSeconds);
bool receiveResponse(MesgBuf *buf , bool checkForOK, u08 waitSeconds);



void  gs_Init(FILE *port)
{
  modemPort = port;

  // Dummy command to clear the buffer
  sendATCommand("at",&def_response, true, 1);

  // Turn off echo
  sendATCommand("ate0", &def_response, true, 1);
  // Enable DHCP CLient Mode
  sendATCommand("at+NDHCP=1", &def_response, true, 5);
  // Enable Station Mode
  sendATCommand("at+WM=0",&def_response, true, 5);
  // Disconnect if previously connected
  sendATCommand("at+WD",&def_response, true, 5);
  // Set up the pre-shared keys
  sendATCommand("at+WPAPSK=ecs-office-net,DonGiovanni", &def_response, true, 20);
  // Connect to the AP
  sendATCommand("at+WA=ecs-office-net", &def_response, true, 20);

  // Set up NTP to point to the global time pool
  // First call forces update right away
  sendATCommand("at+NTIMESYNC=1,216.45.57.38,10,0", &def_response, true, 20);
  _delay_ms(300); // Give it a little time to actually contact an NTP server
  sendATCommand("at+NTIMESYNC=1,216.45.57.38,10,1,3600", &def_response, true, 20);
}

HTTPMesg* gs_send_HTTPMessage(u08 *URL, HTTP_MesgType type, HTTPMesg *mesg)
{

}


int gs_get_NTPTime()
{
  sendATCommand("at+gettime=?",&def_response, true, 1);
  return 0;
}


bool sendATCommand(char *cmd, MesgBuf *response, bool checkOK, u08 waitSeconds)
{
  printf("Send Command-> [%s]", cmd);
  fputs(cmd,modemPort);
  fputs("\n",modemPort);
  bool ret = receiveResponse(response, checkOK, waitSeconds );
  if (response->len > 0) printf(" Response->%s \n", response->buf);
  else printf("No Response\n");
  return ret;
}


bool receiveResponse(MesgBuf *buf , bool checkForOK, u08 waitSeconds)
{
  int ch;
  bool newData = false;

  // Specify the wait in seconds. If check for OK, will return
  // as soon as OK is received regardless of the waitSeconds.
  int timeout_period = waitSeconds * 20;

  buf->len = 0;
  char *dptr = (char *)buf->buf;

  // Delay a little bit so the buffer can fill up
  _delay_ms(350);
  while(timeout_period > 0)
  {
    // Inner delay loop allow chars to be received.
    _delay_ms(50);
    newData = false;
    while((ch = fgetc(modemPort)) != EOF)
    {
      *dptr++ = ch;
      *dptr = 0x00;
      buf->len++;
      newData=true;
      if (buf->len >= buf->capicity-2) break;
    }

    if (checkForOK & newData)
    {
      if (strstr((char*)buf->buf, "OK")) return true;
    }

    // We ran out of buffer space
    if (buf->len >= buf->capicity-2) return false;
    timeout_period--;
  }
  // This is a time out condition, return true if we haven't
  // over run the buffer and there is some data.
  if (buf->len > 0) return true;
  return false;
}




