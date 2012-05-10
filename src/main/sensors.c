#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <util/delay.h>
#include "../avr/driver/i2c.h"
#include "../device/HMC5843/HMC5843.h"
#include "../device/bma180/bma180_i2c.h"
#include "../avr/driver/uart2.h"
#include "../avr/rprintf.h"
#include "../network/gswifi/gs.h"

#include "../lib/timer128.h"
#include "../lib/simpleTime.h"

#include "sensors.h"


typedef void (*sensorQueueFn)(void);

/**
 * initiate a single cycle of data collection
 */
static void _sampleCycle(void);

/**
 * called every tick by internal timer
 */
static void _handleTick(void);

static void _processBMA180(void);
static void _processBMA180Complete(void);
static void _advanceDataPage(void);
static void _transmitPage(u08 pageIn);


static void enqueueSensorFunction(sensorQueueFn fn);
static void processSensorQueue();
static void clearSensorQueue();
static u08 isSensorQueueEmpty();

/**
 * true if module is currently initialized and collecting data from sensors
 */
static u08 _isCollectingData = false;

/**
 * internal data structures
 */
static DataBuffer gDataBuffer;


short connectionId;

void initSensorTimers()
{
	timer2Init();
	timerAttach(TIMER2OVERFLOW_INT, _handleTick);
	timer2SetPrescaler(TIMER2_CLK_DIV1);
}

/**
 * counter to keep track of how many ticks have elapsed since the last second
 */
static u08 _realtimeClockCounter = 0;

static void _handleTick(void)
{
	// reset counter (which has just overflowed to trigger this interrupt) to start
	// at the proper value to scale the timer to the desired frequency (if we didn't
	// do this the default would just be overflowing the 8bit counter (256))
	timer2SetCounter(128);

	// with current 32kHz crystal, no prescale, and 128 overflow,
	// this will get called at 256Hz

	// tell real time clock to update every second
	_realtimeClockCounter++;
	if (_realtimeClockCounter == TIMER_TICKS_PER_SECOND) {
		simpleTimeTickSecond();
		_realtimeClockCounter = 0;
	}

	// 256/5 ~= 51Hz sampling rate

	if (_realtimeClockCounter % 5 == 0)
		_sampleCycle();
}

/**
 * true when currently processing a sample cycle
 */
static u08 _inSampleCycle = false;

static void _sampleCycle(void)
{
	if (!_isCollectingData)
		return;

	if (_inSampleCycle) // still processing previous sample cycle, skip
		return;

	_inSampleCycle = true;

	processSensorQueue();
}

static void _sampleCycleComplete()
{
	_inSampleCycle = false;

	processSensorQueue();

}

/**
 * setup memory buffers and IO for operation
 */
void initSensorModule()
{
  bma180_init();
  bma180_SetRange(r2G);
  bma180_SetBandwidth(bw40Hz);

  // Initialize the Data Buffer.
  gDataBuffer.currPageIn = 0;
  gDataBuffer.currSampleIn = 0;
  for (uint8_t i=0; i<NUM_DATA_PAGES; i++) {
    gDataBuffer.pages[i].numReadings = 0;
    gDataBuffer.pages[i].lockForSending = false;
  }

  enqueueSensorFunction(&_processBMA180);
  enqueueSensorFunction(&_advanceDataPage);
  enqueueSensorFunction(&_sampleCycleComplete);

  // initialization complete, start collecting data!
  _isCollectingData = true;
}

void sensorMainLoop()
{
	while (true)
	{
		// find a filled page that's ready to be transmitted
		u08 filledPageIn = PAGE_INDEX_UNAVAILABLE;
		for (u08 i=0; i<NUM_DATA_PAGES; i++)
		{
			if (gDataBuffer.pages[i].numReadings == SAMPLES_PER_DATA_PAGE)
			{
				filledPageIn = i;
				break;
			}
		}

		if (filledPageIn != PAGE_INDEX_UNAVAILABLE)
			_transmitPage(filledPageIn);

		_delay_ms(100);
	}
}

static void _transmitPage(u08 pageIn)
{
	gDataBuffer.pages[pageIn].lockForSending = TRUE;

	printf("start sending page %d\n", pageIn);

	// start transmit
	u16 dataSize = 700; // SAMPLES_PER_DATA_PAGE * (NUM_SAMPLE_BYTES + 2);

	gs_start_data(dataSize, connectionId);

	for (u08 i=0; i<SAMPLES_PER_DATA_PAGE; i++)
	{
		u08 offsetIn = (SENSOR_ID_BMA180 * SAMPLES_PER_DATA_PAGE)+i;
		u08 *buffer = (u08*)&gDataBuffer.pages[pageIn].data[offsetIn];

		u16 x, y, z;

		x = buffer[1]<<8|buffer[0];

		y = buffer[3]<<8|buffer[2];

		z = buffer[5]<<8|buffer[4];

		char output[50];
		sprintf(output, "%04x%04x%04x\r\n", x, y, z);
		uartSendBuffer(1, output, strlen(output));
		// uartSendBuffer(0, buffer, 6);
	}

	//

	gDataBuffer.pages[pageIn].numReadings = 0;
	gDataBuffer.pages[pageIn].lockForSending = FALSE;
}

static void _processBMA180(void)
{
	if (gDataBuffer.currPageIn == PAGE_INDEX_UNAVAILABLE)
	{
		// all data pages are filled, so we'll need to drop this data sample
		processSensorQueue();
		return;
	}

	uint8_t index = (SENSOR_ID_BMA180 * SAMPLES_PER_DATA_PAGE)+gDataBuffer.currSampleIn;

	// printf("BMA Sample Start\n");
	u08 *buffer = (u08*)&gDataBuffer.pages[gDataBuffer.currPageIn].data[index];
	bma180_ReadSensorData(buffer, _processBMA180Complete);
}

static void _processBMA180Complete(void)
{
	processSensorQueue();
}

static void _advanceDataPage(void)
{
	if (gDataBuffer.pages[gDataBuffer.currPageIn].numReadings == SAMPLES_PER_DATA_PAGE)
	{
		// find next free page
		u08 freePage = PAGE_INDEX_UNAVAILABLE;
		for (u08 i=0; i<NUM_DATA_PAGES; i++)
		{
			if (gDataBuffer.pages[i].numReadings == 0 && !gDataBuffer.pages[i].lockForSending)
			{
				freePage = i;
				break;
			}
		}

		gDataBuffer.currPageIn = freePage;

		// init the new data page if necessary
		if (gDataBuffer.currPageIn != PAGE_INDEX_UNAVAILABLE)
		{
			gDataBuffer.currSampleIn = 0;
			gDataBuffer.pages[gDataBuffer.currPageIn].numReadings = 0;
		}
	}
	else
	{
		gDataBuffer.currSampleIn++;
		gDataBuffer.pages[gDataBuffer.currPageIn].numReadings++;
	}

	// printf("Advance data Page %d, Sample %d\n", gDataBuffer.currPageIn, gDataBuffer.currSampleIn);

	processSensorQueue();
}


/**
 * The Function Queue is a set of routines that are executed sequentially
 * as the previous function ends, then next one in line is run until the
 * last function is called. Once the last function is called, the sequence
 * ends and quietly ends. The Sensor queue is initiated by adding functions
 * functions to the queue then calling processSensorQueue as each function
 * ends, it should call processSensorQueue to call the next function inline.
 *
 */
#define SENSOR_QUEUE_SIZE 10

static sensorQueueFn _sensorFnQueue[SENSOR_QUEUE_SIZE];
static uint8_t _queueIndex = 0;
static uint8_t _queueSize = 0;


static void enqueueSensorFunction(sensorQueueFn fn)
{
  if (_queueSize == SENSOR_QUEUE_SIZE)
    return;  // error condition, queue is full

  _sensorFnQueue[_queueSize] = fn;
  _queueSize++;
}


/**
 *
 */
static void processSensorQueue(void)
{
  if (_queueIndex == _queueSize) {
      // We have hit the end of the line, reset the index to
      // prepare for the next run and quietly end.
      _queueIndex = 0;
  }
  else {
    // run the next function in the queue
    sensorQueueFn toRun = _sensorFnQueue[_queueIndex];
    _queueIndex++;
    toRun();
  }
}

static void clearSensorQueue()
{
  _queueIndex = 0;
  _queueSize = 0;
}

static uint8_t isSensorQueueEmpty()
{
  return _queueSize == 0;
}

