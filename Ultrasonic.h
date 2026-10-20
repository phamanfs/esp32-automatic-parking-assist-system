#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

enum UltrasonicSamplingMode {
  ULTRASONIC_MODE_ALL,
  ULTRASONIC_MODE_SEARCH,
  ULTRASONIC_MODE_PARKING
};

/************************************************
 *              INITIALIZE
 ***********************************************/

void ultrasonicInit();

// Call this frequently from loop(). It samples only one sensor every
// ULTRASONIC_SAMPLE_INTERVAL_MS and keeps the latest filtered valid value.
void ultrasonicUpdate();

// Selects which sensors are prioritised by ultrasonicUpdate().
void ultrasonicSetSamplingMode(UltrasonicSamplingMode mode);

/************************************************
 *          PUBLIC FUNCTIONS
 ***********************************************/

float getFrontDistance();

float getRearDistance();

float getLeftDistance();

float getRightDistance();

#endif
