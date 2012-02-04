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

typedef void (*sensorQueueFn)(void);

static sensorQueueFn _sensorFnQueue[SENSOR_QUEUE_SIZE];
static uint8_t _queueIndex = 0;
static uint8_t _queueSize = 0;

/**
 * internal function declarations
 */
static void _initBuffer();
static void _saveSensorSample(uint8_t *data, uint8_t sensorId);

static void _readHMC5843();
static void _processHMC5843();
static void _HMC5843_start_single();

static void _readBMA180();
static void _BMA180_SetupSensorRead();
static void _processBMA180();

static void _bma180_init();
static void _bma180_set_ee_w();
static void _bma180_SetRange();
static void _bma180_SetBandwidth();

static void _bma180_WriteReg(uint8_t address, uint8_t value);
static void _bma180_ModReg_step1(uint8_t address, uint8_t mask, uint8_t value);
static void _bma180_ModReg_step2();
static void _bma180_ModReg_step3();


/**
 * internal data structures
 */
static DataBuffer gDataBuffer;

static uint8_t cmd_continious[] = {0x02, 0x00};
static uint8_t cmd_single[] = {0x02, 0x01};

static uint8_t _receiveSampleBuffer[NUM_SAMPLE_BYTES];

// command issued to bma180 over i2c to setup state for reading sensor data
static uint8_t _bma180_addr = 0x02;


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

	// register callback for i2c driver
	registerI2cSendCompleteCallback(&sensorQueueFnComplete);
	registerI2cReceiveCompleteCallback(&sensorQueueFnComplete);

	// setup data pages for sample data
	_initBuffer();

	// init sensors

	// HMC5843
	enqueueSensorQueue(&_HMC5843_start_single);

	// BMA180
	enqueueSensorQueue(&_bma180_init);
	enqueueSensorQueue(&_bma180_set_ee_w);
	// 2012-0203 rchan - the ModReg operations block waiting on data from the BMA180
	// in "step 2" (data is never received)
	/*
	enqueueSensorQueue(&_bma180_SetRange);
	enqueueSensorQueue(&_bma180_ModReg_step2);
	enqueueSensorQueue(&_bma180_ModReg_step3);
	enqueueSensorQueue(&_bma180_SetBandwidth);
	enqueueSensorQueue(&_bma180_ModReg_step2);
	enqueueSensorQueue(&_bma180_ModReg_step3);
	*/

	enqueueSensorQueue(&_BMA180_SetupSensorRead);

	startSensorQueue();
	_delay_ms(1000);
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

	// 2012-0203 rchan - enabling either the HMC5843
	// or BMA180 sections appears to be ok, but when both
	// are running something blocks

	// HMC5843
	/*
	enqueueSensorQueue(&_readHMC5843);
	enqueueSensorQueue(&_processHMC5843);
	enqueueSensorQueue(&_HMC5843_start_single);
	*/

	// BMA180
	enqueueSensorQueue(&_readBMA180);
	enqueueSensorQueue(&_processBMA180);
	enqueueSensorQueue(&_BMA180_SetupSensorRead);

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
	rprintf("Start sensor readings\n");
	uartSendTxBuffer(0);
	_delay_ms(1000);

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
	long x, y, z;
	x = (_receiveSampleBuffer[0] << 8) | _receiveSampleBuffer[1];
	y = (_receiveSampleBuffer[2] << 8) | _receiveSampleBuffer[3];
	z = (_receiveSampleBuffer[4] << 8) | _receiveSampleBuffer[5];
	rprintf("hmc5843 x=%d y=%d z=%d\n", x, y, z);
	uartSendTxBuffer(0);

	// signal fn complete
	sensorQueueFnComplete();
}

static void _HMC5843_start_single()
{
  i2cMasterSend(0x3c, sizeof(cmd_continious), cmd_single);
}

static void _readBMA180()
{
	i2cMasterReceive(BMA180_DeviceID, sizeof(uint8_t) * NUM_SAMPLE_BYTES, _receiveSampleBuffer);
}

static void _BMA180_SetupSensorRead()
{
	i2cMasterSend(BMA180_DeviceID, sizeof(uint8_t), &_bma180_addr);
}

static void _processBMA180()
{
	_saveSensorSample(_receiveSampleBuffer, SENSOR_ID_BMA180);

	// TODO - remove debugging
    uint16_t x, y, z;
    x = _receiveSampleBuffer[1]<<8|_receiveSampleBuffer[0];
    x = x >> 2;

    y = _receiveSampleBuffer[3]<<8|_receiveSampleBuffer[2];
    y = y >> 2;

    z = _receiveSampleBuffer[5]<<8|_receiveSampleBuffer[4];
    z= z >> 2;

    rprintf("bma180 X=%d, Y=%d, Z=%d\n", x, y, z);
	uartSendTxBuffer(0);

	// signal fn complete
	sensorQueueFnComplete();
}

static void _bma180_init()
{
	// TODO - lookup these and comment properly
	_bma180_WriteReg(0x10, 0xB6);
}

static void _bma180_set_ee_w()
{
	_bma180_WriteReg(0x0d, 0B00010000);
}

static void _bma180_SetRange()
{
	enum Range range = r2G;
	_bma180_ModReg_step1(0x35, 0B11110001, ((uint8_t)range) << 1);
}

static void _bma180_SetBandwidth()
{
	enum Bandwidth bw = bw40Hz;
	_bma180_ModReg_step1(0x20, 0B00001111, ((uint8_t) bw) << 4);
}


static uint8_t _bma180_reg_write_buf[2];
static void _bma180_WriteReg(uint8_t address, uint8_t value)
{
	_bma180_reg_write_buf[0] = address;
	_bma180_reg_write_buf[1] = value;
	i2cMasterSend(BMA180_DeviceID, sizeof(_bma180_reg_write_buf), _bma180_reg_write_buf);
}

static uint8_t _bma180_ModRegAddr;
static uint8_t _bma180_ModRegMask;
static uint8_t _bma180_ModRegValue;
static uint8_t _bma180_ModRegCurrValue;

static void _bma180_ModReg_step1(uint8_t address, uint8_t mask, uint8_t value)
{
	_bma180_ModRegAddr = address;
	_bma180_ModRegMask = mask;
	_bma180_ModRegValue = value;

	// step 1 - initiate reg read
	i2cMasterSend(BMA180_DeviceID, 1, &_bma180_ModRegAddr);
}

static void _bma180_ModReg_step2()
{
	i2cMasterReceive(BMA180_DeviceID, 1, &_bma180_ModRegCurrValue);
}

static void _bma180_ModReg_step3()
{
	uint8_t newVal;
	newVal = (_bma180_ModRegCurrValue & _bma180_ModRegMask) | _bma180_ModRegValue;

	_bma180_WriteReg(_bma180_ModRegAddr, newVal);
}

static void _readL3G4200D()
{
	//
}
