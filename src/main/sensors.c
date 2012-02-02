#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "../avr/driver/i2c.h"
#include "../device/HMC5843/HMC5843.h"
#include "../device/bma180/bma180_i2c.h"
#include "../avr/driver/uart2.h"
#include "../avr/rprintf.h"


#include "sensors.h"

/**
 * function queue used to chain interrupt-driven asynchronous callbacks
 */

#define SENSOR_QUEUE_SIZE 10

#define SENSOR_ID_HMC5843 0


typedef void (*sensorQueueFn)(void);

static sensorQueueFn _sensorFnQueue[SENSOR_QUEUE_SIZE];
static uint8_t _queueIndex = 0;
static uint8_t _queueSize = 0;

/**
 * internal function declarations
 */
static void _readHMC5843();
static void _initBuffer();
static void _saveSensorSample(uint8_t *data, uint8_t sensorId);

static void _processHMC5843();
static void _HMC5843_start_single();

/**
 * internal data structures
 */
static DataBuffer gDataBuffer;

static uint8_t cmd_continious[] = {0x02, 0x00};
static uint8_t cmd_single[] = {0x02, 0x01};

static uint8_t _receiveSampleBuffer[NUM_SAMPLE_BYTES];


/**
 * function queue implementation
 */

static void enqueueSensorQueue(sensorQueueFn fn)
{
	if (_queueSize == SENSOR_QUEUE_SIZE)
		return;  // error condition, queue is full

	_sensorFnQueue[_queueSize] = fn;
	_queueSize++;
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

static void startSensorQueue()
{
	if (isSensorQueueEmpty())
		return;

	sensorQueueFn toRun = _sensorFnQueue[_queueIndex];
	_queueIndex++;
	toRun();
}

static void sensorQueueFnComplete()
{
	if (_queueIndex == _queueSize) {
		// completed the entire queue, time to celebrate
		clearSensorQueue();
	}
	else {
		// run the next function in the queue
		sensorQueueFn toRun = _sensorFnQueue[_queueIndex];
		_queueIndex++;
		toRun();
	}
}


/**
 * setup memory buffers and IO for operation
 */
void initSensorModule()
{
	// initialize serial IO for debugging
	uartInit();
	rprintfInit(uart0AddToTxBuffer);

	// init sensors
	_HMC5843_start_single();

	// setup data pages for sample data
	_initBuffer();

	// register callback for i2c driver
	registerI2cSendCompleteCallback(&sensorQueueFnComplete);
	registerI2cReceiveCompleteCallback(&sensorQueueFnComplete);

	rprintf("Sensor code initialized\n");
	_delay_ms(1000);
	uartSendTxBuffer(0);
}

/**
 * initialize all data pages
 */
static void _initBuffer()
{
	gDataBuffer.currPage = 0;
	for (uint8_t i=0; i<NUM_DATA_PAGES; i++) {
		gDataBuffer.pages[i].numReadings = 0;
		gDataBuffer.pages[i].lockForSending = false;
	}
}

void captureData()
{
	// populate sensor fn queue
	enqueueSensorQueue(&_readHMC5843);
	enqueueSensorQueue(&_processHMC5843);
	enqueueSensorQueue(&_HMC5843_start_single);
	enqueueSensorQueue(&clearSensorQueue);

	// start executing...
	startSensorQueue();
}

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

void sensorMainLoop()
{
	while(true) {
		captureData();
		_delay_ms(500);
	}
}


static void _readHMC5843()
{
	i2cMasterReceive(0x3c, sizeof(uint8_t) * NUM_SAMPLE_BYTES, _receiveSampleBuffer);
}

/**
 * properly save the sample read from the HMC5843 using the _readHMC5843 function
 */
static void _processHMC5843()
{
	_saveSensorSample(_receiveSampleBuffer, SENSOR_ID_HMC5843);

	// TODO - remove debugging
	rprintf("\n\n\n\n");
	long v;
	v = (_receiveSampleBuffer[0] << 8) | _receiveSampleBuffer[1];
	rprintf("x=%d, ", v);

	v = (_receiveSampleBuffer[2] << 8) | _receiveSampleBuffer[3];
	rprintf("y=%d, ", v );

	v = (_receiveSampleBuffer[4] << 8) | _receiveSampleBuffer[5];
	rprintf("z=%d\n ",v );

	uartSendTxBuffer(0);
}

static void _HMC5843_start_single()
{
  i2cMasterSend(0x3c, sizeof(cmd_continious), cmd_single);
}
void HMC5843_start_continious(void)
{
	i2cMasterSendNI(0x3c, sizeof(cmd_continious), cmd_continious);
}

static void _readBMA180()
{
	//
}

static void _readL3G4200D()
{
	//
}
