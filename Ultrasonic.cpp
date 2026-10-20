#include "Ultrasonic.h"

#include "Pins.h"

static const unsigned long ULTRASONIC_SAMPLE_INTERVAL_MS = 60;
static const uint8_t FILTER_SAMPLE_COUNT = 5;

enum SensorIndex {
  FRONT_SENSOR,
  REAR_SENSOR,
  LEFT_SENSOR,
  RIGHT_SENSOR,
  SENSOR_COUNT
};

static const uint8_t trigPins[SENSOR_COUNT] = {
  FRONT_TRIG, REAR_TRIG, LEFT_TRIG, RIGHT_TRIG
};
static const uint8_t echoPins[SENSOR_COUNT] = {
  FRONT_ECHO, REAR_ECHO, LEFT_ECHO, RIGHT_ECHO
};

static float samples[SENSOR_COUNT][FILTER_SAMPLE_COUNT];
static uint8_t sampleCount[SENSOR_COUNT] = {0};
static uint8_t sampleWriteIndex[SENSOR_COUNT] = {0};
static float filteredDistance[SENSOR_COUNT] = {-1, -1, -1, -1};
static UltrasonicSamplingMode samplingMode = ULTRASONIC_MODE_ALL;
static uint8_t sampleSequenceIndex = 0;
static unsigned long lastSampleTime = 0;

static const uint8_t allSensorSequence[] = {
  FRONT_SENSOR, REAR_SENSOR, LEFT_SENSOR, RIGHT_SENSOR
};
static const uint8_t searchSensorSequence[] = {
  RIGHT_SENSOR, RIGHT_SENSOR, RIGHT_SENSOR, FRONT_SENSOR
};
static const uint8_t parkingSensorSequence[] = {
  FRONT_SENSOR, REAR_SENSOR
};

/************************************************
 *          PRIVATE FUNCTION
 ***********************************************/

static float readDistance(uint8_t trigPin, uint8_t echoPin);

/************************************************
 *          INITIALIZE
 ***********************************************/

void ultrasonicInit() {
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(REAR_TRIG, OUTPUT);
  pinMode(REAR_ECHO, INPUT);

  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);

  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);

  digitalWrite(FRONT_TRIG, LOW);
  digitalWrite(REAR_TRIG, LOW);
  digitalWrite(LEFT_TRIG, LOW);
  digitalWrite(RIGHT_TRIG, LOW);

  lastSampleTime = millis() - ULTRASONIC_SAMPLE_INTERVAL_MS;
}

void ultrasonicSetSamplingMode(UltrasonicSamplingMode mode) {
  if (samplingMode != mode) {
    samplingMode = mode;
    sampleSequenceIndex = 0;
  }
}

static float readDistance(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  unsigned long duration =
    pulseIn(echoPin, HIGH, 30000);

  if (duration == 0)
    return -1;

  return duration * 0.0343f / 2.0f;
}

static float medianOfSamples(uint8_t sensor) {
  float sorted[FILTER_SAMPLE_COUNT];
  for (uint8_t i = 0; i < FILTER_SAMPLE_COUNT; ++i) {
    sorted[i] = samples[sensor][i];
  }

  for (uint8_t i = 1; i < FILTER_SAMPLE_COUNT; ++i) {
    float value = sorted[i];
    int8_t j = i - 1;
    while (j >= 0 && sorted[j] > value) {
      sorted[j + 1] = sorted[j];
      --j;
    }
    sorted[j + 1] = value;
  }
  return sorted[FILTER_SAMPLE_COUNT / 2];
}

void ultrasonicUpdate() {
  if (millis() - lastSampleTime < ULTRASONIC_SAMPLE_INTERVAL_MS) {
    return;
  }

  lastSampleTime = millis();
  const uint8_t* sequence = allSensorSequence;
  uint8_t sequenceLength = sizeof(allSensorSequence);

  if (samplingMode == ULTRASONIC_MODE_SEARCH) {
    sequence = searchSensorSequence;
    sequenceLength = sizeof(searchSensorSequence);
  } else if (samplingMode == ULTRASONIC_MODE_PARKING) {
    sequence = parkingSensorSequence;
    sequenceLength = sizeof(parkingSensorSequence);
  }

  const uint8_t sensor = sequence[sampleSequenceIndex];
  sampleSequenceIndex = (sampleSequenceIndex + 1) % sequenceLength;

  const float distance = readDistance(trigPins[sensor], echoPins[sensor]);
  if (distance <= 0.0f) {
    // A timeout is not an obstacle and must not overwrite a good reading.
    return;
  }

  samples[sensor][sampleWriteIndex[sensor]] = distance;
  sampleWriteIndex[sensor] =
    (sampleWriteIndex[sensor] + 1) % FILTER_SAMPLE_COUNT;

  if (sampleCount[sensor] < FILTER_SAMPLE_COUNT) {
    ++sampleCount[sensor];
  }

  // Do not expose a measurement until five valid samples have been collected.
  if (sampleCount[sensor] == FILTER_SAMPLE_COUNT) {
    filteredDistance[sensor] = medianOfSamples(sensor);
  }
}

float getFrontDistance() {
  return filteredDistance[FRONT_SENSOR];
}

float getRearDistance() {
  return filteredDistance[REAR_SENSOR];
}

float getLeftDistance() {
  return filteredDistance[LEFT_SENSOR];
}

float getRightDistance() {
  return filteredDistance[RIGHT_SENSOR];
}
