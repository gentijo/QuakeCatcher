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
void flushRxBuffer();



void  gs_Init(FILE *port, char *networkName, char *pwd)
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

  // Close any previous connections
  sendATCommand("at+NCLOSEALL",&def_response, true, 5);

  // Set up the pre-shared keys
  char cmdBuffer[100];
  sprintf(cmdBuffer, "at+WPAPSK=%s,%s", networkName, pwd);
  sendATCommand(cmdBuffer, &def_response, true, 20);
  // Connect to the AP
  sprintf(cmdBuffer, "at+WA=%s", networkName);
  sendATCommand(cmdBuffer, &def_response, true, 20);

  sendATCommand(
      "at+BDATA=1", &def_response, true, 1);

  // Set up NTP to point to the global time pool
  // First call forces update right away
  /*
  sendATCommand("at+NTIMESYNC=1,216.45.57.38,10,0", &def_response, true, 5);
  _delay_ms(300); // Give it a little time to actually contact an NTP server
  sendATCommand("at+NTIMESYNC=1,216.45.57.38,10,1,3600", &def_response, true, 5);
  */
}

int  gs_open_connection(char *address, char *port)
{
  int CID = 0;

  printf("Opening Connection\n");

  char cmdBuffer[100];
  sprintf(cmdBuffer, "at+NCTCP=%s,%s", address, port);

  if (!sendATCommand(cmdBuffer, &def_response, true, 20)) CID = -1;
  else
  {
    printf("response-> %s", def_resp_buf);
    sscanf(def_resp_buf, "%d", &CID);
  }

  printf("Connection open done CID=%d", CID);

  return CID;
}

bool gs_start_data(u16 length, short connectionId)
{
	printf("Sending Data Z%04d\n",length);
	// fprintf(modemPort, "\x1bZ%04d",length);
	fprintf(modemPort, "\x1bZ%d%04d",connectionId, length);

	return true;
}

FILE *gs_get_modem_port()
{
	return modemPort;
}

bool gs_send_data(int connid, MesgBuf *txdata, MesgBuf *rxdata)
{
	flushRxBuffer();
	gs_start_data(txdata->len, connid);
	for (int x=0; x< txdata->len; x++) fputc(txdata->buf[x],modemPort);
	return true;
}

bool  gs_close_connection(int cid)
{
  char command[100];
  sprintf(command, "AT+NCLOSE=%d", cid);
  return sendATCommand(command, &def_response, true, 20);
}

time_t gs_get_NTPTime()
{
  struct tm time;
  int month, day, year, hour, min, sec;

  sendATCommand("at+gettime=?",&def_response, true, 1);
  //28/4/2012,5:3:7,1335589387249
  sscanf( def_resp_buf,
      "%d/%d/%d,%d:%d:%d",
      &day, &month, &year,
      &hour, &min, &sec);
  time.tm_year = year -1900;

  printf(
      "\nTime %d/%d/%d %d:%d:%d\n",
      month, day, year,
      hour, min, sec);

  time.tm_mon = (u08)month;
  time.tm_mday = (u08)day;
  time.tm_year = (u08)year -1900;
  time.tm_hour = (u08)hour;
  time.tm_min = (u08)min;
  time.tm_sec = (u08)sec;
  time.tm_zone = "GMT";

  time_t ntime = mktime(&time);
  printf("%ld\n", ntime);

  return ntime;
}


bool sendATCommand(char *cmd, MesgBuf *response, bool checkOK, u08 waitSeconds)
{
  flushRxBuffer();

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


void flushRxBuffer()
{

  // Delay a little bit so the buffer can fill up
  _delay_ms(350);
  while(fgetc(modemPort) != EOF)
  {
    _delay_ms(50);
  }
}



