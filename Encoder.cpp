#include "Encoder.h"

#include "Config.h"
#include "Pins.h"

static volatile unsigned long pulseCount = 0;
static portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;

static void IRAM_ATTR onEncoderPulse() {
  portENTER_CRITICAL_ISR(&encoderMux);
  ++pulseCount;
  portEXIT_CRITICAL_ISR(&encoderMux);
}

void encoderInit() {
  // GPIO34 is input-only and has no internal pull-up. The TH106 signal was
  // verified stable in hardware, so it is used as a plain digital input.
  pinMode(ENCODER_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), onEncoderPulse, RISING);
}

unsigned long getEncoderPulseCount() {
  portENTER_CRITICAL(&encoderMux);
  const unsigned long count = pulseCount;
  portEXIT_CRITICAL(&encoderMux);
  return count;
}

float encoderPulsesToCm(unsigned long pulses) {
  if (ENCODER_PULSES_PER_CM <= 0.0f) {
    return 0.0f;
  }
  return pulses / ENCODER_PULSES_PER_CM;
}
