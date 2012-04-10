/**
 * simpletime.h
 * ------------
 * A trivial counter that's intended to be a shared timestamp resource
 * (this module doesn't actually keep time itself; it depends on an
 * external source to call simpleTimeTickSecond every... second)
 */

#ifndef SIMPLETIME_H_
#define SIMPLETIME_H_

#include "../include/global.h"


u32 getSimpleTime(void);

void setSimpleTime(u32 currTime);

/**
 * should be called every second by an external routine to update the global
 * time value
 */
void simpleTimeTickSecond(void);

#endif
