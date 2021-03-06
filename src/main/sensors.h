#ifndef SENSORS_H_
#define SENSORS_H_


#include "global.h"
// All the sensors 2 bytes per axis, 3 axis

#define NUM_SENSORS 2
#define SAMPLES_PER_SECOND 50
#define SAMPLES_PER_DATA_PAGE 50
#define NUM_DATA_PAGES 10

#define SENSOR_ID_BMA180 0
#define SENSOR_ID_HMC5843 1

#define NUM_SAMPLE_BYTES 6 // sizeof(struct_SensorReading)

/**
 * sentinel value indicating that no data pages are currently available for writing
 */
#define PAGE_INDEX_UNAVAILABLE 255

/**
 * the number of times _handleTick is called in a second
 * NOTE: this is not a setting; if you want a different frequency you
 * must still change the prescale and counter values in initSensorTimers
 * and _handleTick to the desired values
 */
#define TIMER_TICKS_PER_SECOND 255

typedef struct struct_SensorSample {
	uint8_t bytes[2];
} SensorSample;

typedef struct struct_SensorReading {
	SensorSample x;
	SensorSample y;
	SensorSample z;
} SensorReading;

typedef struct struct_DataPage {
	u32 startTime;
	uint8_t numReadings;
	bool lockForSending;
	SensorReading data[NUM_SENSORS * SAMPLES_PER_DATA_PAGE];
} DataPage;

typedef struct struct_DataBuffer {
	DataPage pages[NUM_DATA_PAGES];
	u08 currPageIn;
	u08 currSampleIn;
} DataBuffer;

typedef void (*sensorReadFn)(uint8_t *);

/**
 * initialize the timers required for sensor module operation
 * NOTE: this should be called before interrupts are enabled (e.g. sei call)
 */
void initSensorTimers();

void initSensorModule();

void captureData();

void sensorMainLoop();

extern short connectionId;

#endif
