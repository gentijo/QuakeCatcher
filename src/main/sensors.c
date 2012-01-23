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
 * internal function declarations
 */
static void _readHMC5843(uint8_t *buffer);
static void _initBuffer();
static void _saveSensorSample(uint8_t *data, uint8_t sensorId);

static void _HMC5843_start_single();

/**
 * internal data structures
 */
static DataBuffer gDataBuffer;
static sensorReadFn sensorFunctions[NUM_SENSORS];

static uint8_t cmd_continious[] = {0x02, 0x00};
static uint8_t cmd_single[] = {0x02, 0x01};

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

	// register all functions to read in sensor data
	sensorFunctions[0] = &_readHMC5843;

	_initBuffer();

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
	uint8_t buffer[NUM_SAMPLE_BYTES]; // 3 axis * 16bit values
	for (uint8_t i=0; i<NUM_SENSORS; i++) {
		(sensorFunctions[i])(buffer);

		_saveSensorSample(buffer, i);
	}
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
		_delay_ms(100);
		captureData();
	}
}


static void _readHMC5843(uint8_t *buffer)
{
	i2cMasterReceiveNI(0x3c, sizeof(uint8_t) * NUM_SAMPLE_BYTES, buffer);
}

static void _HMC5843_start_single()
{
  i2cMasterSendNI(0x3c, sizeof(cmd_continious), cmd_single);
}
void HMC5843_start_continious(void)
{
	i2cMasterSendNI(0x3c, sizeof(cmd_continious), cmd_continious);
}

static void _readBMA180(uint8_t *buffer)
{
	//
}

static void _readL3G4200D(uint8_t *buffer)
{
	//
}
