// time.h
// ------
// adapted for AVR from original source
// attribution below

//
// time.h
//
// Time routines
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//


#include "../include/global.h"

#ifndef TIME_H
#define TIME_H

#define CLOCKS_PER_SEC  1000

typedef u32 time_t;

#ifndef _TM_DEFINED
#define _TM_DEFINED

struct tm
{
  u08 tm_sec;                   // Seconds after the minute [0, 59]
  u08 tm_min;                   // Minutes after the hour [0, 59]
  u08 tm_hour;                  // Hours since midnight [0, 23]
  u08 tm_mday;                  // Day of the month [1, 31]
  u08 tm_mon;                   // Months since January [0, 11]
  u08 tm_year;                  // Years since 1900
  u08 tm_wday;                  // Days since Sunday [0, 6]
  u08 tm_yday;                  // Days since January 1 [0, 365]
  u08 tm_isdst;                 // Daylight Saving Time flag
  u08 tm_gmtoff;                // Seconds east of UTC
  char *tm_zone;                // Timezone abbreviation
};

#endif

#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED

struct timeval
{
  u32 tv_sec;                  // Seconds
  u32 tv_usec;                 // Microseconds
};

#endif

#ifndef _TIMEZONE_DEFINED
#define _TIMEZONE_DEFINED

struct timezone
{
  u32 tz_minuteswest;           // Minutes west of Greenwich
  u32 tz_dsttime;               // Type of daylight saving correction
};

#endif

extern u32 _daylight;     // Non-zero if daylight savings time is used
extern u32 _dstbias;     // Offset for Daylight Saving Time
extern u32 _timezone;    // Difference in seconds between GMT and local time
extern char *_tzname[2];  // Standard/daylight savings time zone names

#define difftime(time2, time1) ((u32)((time2) - (time1)))

time_t mktime(struct tm *tmbuf);


#endif
