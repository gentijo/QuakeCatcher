
#include "simpleTime.h"

static u32 _currTime = 0;


u32 getSimpleTime(void)
{
	return _currTime;
}

void setSimpleTime(u32 currTime)
{
	_currTime = currTime;
}

/**
 * should be called every second by an external routine to update the global
 * time value
 */
void simpleTimeTickSecond(void)
{
	if (_currTime == 0)
		return;  // time not yet initialized, ignore ticks

	_currTime++;
}
