#ifndef SENSORS_H_
#define SENSORS_H_

// All the sensors 2 bytes per axis, 3 axis

#define NUM_SENSORS 2
#define SAMPLES_PER_SECOND 50
#define SAMPLES_PER_DATA_PAGE 50
#define NUM_DATA_PAGES 10

#define SENSOR_ID_HMC5843 0
#define SENSOR_ID_BMA180 1

#define NUM_SAMPLE_BYTES 6 // sizeof(struct_SensorReading)

typedef struct struct_SensorSample {
	uint8_t bytes[2];
} SensorSample;

typedef struct struct_SensorReading {
	SensorSample x;
	SensorSample y;
	SensorSample z;
} SensorReading;

typedef struct struct_DataPage {
	uint8_t startTime[4];
	uint8_t numReadings;
	bool lockForSending;
	SensorReading data[NUM_SENSORS * SAMPLES_PER_DATA_PAGE];
} DataPage;

typedef struct struct_DataBuffer {
	DataPage pages[NUM_DATA_PAGES];
	uint8_t currPage;
} DataBuffer;

typedef void (*sensorReadFn)(uint8_t *);

void initSensorModule();

void captureData();

void sensorMainLoop();

#endif
