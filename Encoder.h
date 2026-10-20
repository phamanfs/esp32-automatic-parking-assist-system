#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

void encoderInit();
unsigned long getEncoderPulseCount();
float encoderPulsesToCm(unsigned long pulses);

#endif
