#ifndef PARKING_DETECTOR_H
#define PARKING_DETECTOR_H

#include <Arduino.h>

void parkingDetectorInit();

// Begins a fresh scan using the supplied minimum opening length in cm.
void startParkingDetection(float minimumSpaceCm);

void parkingDetectorUpdate();

bool parkingSpaceFound();

void resetParkingDetector();

float getGapLength();

unsigned long getGapTime();

#endif
