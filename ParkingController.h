#ifndef PARKING_CONTROLLER_H
#define PARKING_CONTROLLER_H

#include <Arduino.h>

void parkingControllerInit();

void parkingControllerUpdate();

void startParallelParking();

void startPerpendicularParking();

void cancelParallelParking();

bool parkingFinished();

#endif
