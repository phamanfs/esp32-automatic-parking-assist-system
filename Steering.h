#ifndef STEERING_H
#define STEERING_H

#include <Arduino.h>

/************************************************
 *              INITIALIZE
 ***********************************************/

void steeringInit();

/************************************************
 *              STEERING
 ***********************************************/

// Điều khiển servo theo giá trị PWM
void steer(uint16_t pwmValue);

// Các hàm tiện lợi
void steerCenter();

void steerLeft();

void steerRight();

#endif
