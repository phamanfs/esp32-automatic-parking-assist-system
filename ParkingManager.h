#ifndef PARKING_MANAGER_H
#define PARKING_MANAGER_H

#include <Arduino.h>

void parkingManagerInit();

void parkingManagerUpdate();

void startAutoParking();

void startAutoPerpendicularParking();

void cancelAutoParking();

bool autoParkingRunning();

#endif
