#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "../avr/driver/i2c.h"
#include "../device/HMC5843/HMC5843.h"
#include "../device/bma180/bma180_i2c.h"
#include "../avr/driver/uart2.h"
#include "../avr/rprintf.h"

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


void initSensorTimers()
{
	timer0Init();
	timerAttach(TIMER0OVERFLOW_INT, _handleTick);
	timer0SetPrescaler(TIMER_CLK_DIV1024);
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
	timer0SetCounter(0); // TODO - currently using system clock; no need to set a value here

	// with current 18mhz (18432000) system clock, 1024 prescale, and 256 overflow,
	// this will get called at about 70hz

	// tell real time clock to update every second
	_realtimeClockCounter++;
	if (_realtimeClockCounter == TIMER_TICKS_PER_SECOND) {
		simpleTimeTickSecond();
		_realtimeClockCounter = 0;
	}

	printf("%d\n", _realtimeClockCounter);

	_sampleCycle();  // sample at 70hz
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
  gDataBuffer.currPageOut = 0;
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
  while(true) {
	  // TODO - place logic for transmitting completed data pages here
    _delay_ms(500);
  }
}

static void _processBMA180(void)
{
  uint8_t index = (SENSOR_ID_BMA180 * SAMPLES_PER_DATA_PAGE)+gDataBuffer.currSampleIn;

  // printf("BMA Sample Start\n");
  u08 *buffer = (u08*)&gDataBuffer.pages[gDataBuffer.currPageIn].data[index];
  bma180_ReadSensorData(buffer, _processBMA180Complete);
}

static void _processBMA180Complete(void)
{
  uint16_t x, y, z;

  uint8_t index = (SENSOR_ID_BMA180 * SAMPLES_PER_DATA_PAGE)+gDataBuffer.currSampleIn;
  u08 *buffer = (u08*)&gDataBuffer.pages[gDataBuffer.currPageIn].data[index];

  x = buffer[1]<<8|buffer[0];
  x = x >> 2;

  y = buffer[3]<<8|buffer[2];
  y = y >> 2;

  z = buffer[5]<<8|buffer[4];
  z= z >> 2;

  printf("BMA180 - X=%d, Y=%d, Z=%d\n", x, y, z);

  processSensorQueue();
}

static void _advanceDataPage(void)
{
  gDataBuffer.currSampleIn++;
  if (gDataBuffer.currSampleIn > SAMPLES_PER_DATA_PAGE)
  {
      gDataBuffer.currSampleIn = 0;
      gDataBuffer.currPageIn++;
      if (gDataBuffer.currPageIn > NUM_DATA_PAGES)
      {
          gDataBuffer.currPageIn = 0;
      }
  }

  printf("Advance data Page %d, Sample %d\n", gDataBuffer.currPageIn, gDataBuffer.currSampleIn);

  processSensorQueue();
}

/*
static void _saveSensorSample(uint8_t *data, uint8_t sensorId)
{
	// Find the next open page to record this data into
	uint8_t pagesTried = 0;
	uint8_t startingPage = gDataBuffer.currPage;
	while (gDataBuffer.pages[gDataBuffer.currPage].lockForSending || gDataBuffer.pages[gDataBuffer.currPage].numReadings == SAMPLES_PER_DATA_PAGE) {
		gDataBuffer.currPage = (gDataBuffer.currPage + 1) % NUM_DATA_PAGES;
		pagesTried++;
		if (pagesTried == NUM_DATA_PAGES) {
			// all pages currently filled or in-use... drop data
			return;
		}
	}

	// init the new data page if necessary
	if (startingPage != gDataBuffer.currPage) {
		gDataBuffer.pages[gDataBuffer.currPage].numReadings = 0;
		// TODO - set start time for page
		rprintf("started filling page %d\n", gDataBuffer.currPage);
		uartSendTxBuffer(0);
	}

	// save data
	// TODO - control byte endian-ness, etc here
	gDataBuffer.pages[gDataBuffer.currPage].data[(gDataBuffer.pages[gDataBuffer.currPage].numReadings * NUM_SENSORS)+sensorId].x.bytes[0] = data[0];
	gDataBuffer.pages[gDataBuffer.currPage].data[(gDataBuffer.pages[gDataBuffer.currPage].numReadings * NUM_SENSORS)+sensorId].x.bytes[1] = data[1];
	gDataBuffer.pages[gDataBuffer.currPage].data[(gDataBuffer.pages[gDataBuffer.currPage].numReadings * NUM_SENSORS)+sensorId].y.bytes[0] = data[2];
	gDataBuffer.pages[gDataBuffer.currPage].data[(gDataBuffer.pages[gDataBuffer.currPage].numReadings * NUM_SENSORS)+sensorId].y.bytes[1] = data[3];
	gDataBuffer.pages[gDataBuffer.currPage].data[(gDataBuffer.pages[gDataBuffer.currPage].numReadings * NUM_SENSORS)+sensorId].z.bytes[0] = data[4];
	gDataBuffer.pages[gDataBuffer.currPage].data[(gDataBuffer.pages[gDataBuffer.currPage].numReadings * NUM_SENSORS)+sensorId].z.bytes[1] = data[5];
	gDataBuffer.pages[gDataBuffer.currPage].numReadings++;
}
*/


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

