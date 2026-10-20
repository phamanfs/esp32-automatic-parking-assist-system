#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

/************************************************
 *              MOTOR FUNCTIONS
 ***********************************************/

void motorInit();

void setMotorSpeed(uint8_t speed);

void forward(uint8_t speed);

void backward(uint8_t speed);

void stopMotor();

#endif