#include "Steering.h"

#include "Pins.h"
#include "Config.h"

/************************************************
 *          INITIALIZE
 ***********************************************/

void steeringInit() {
  // PWM Servo
  ledcSetup(2, SERVO_PWM_FREQ, SERVO_PWM_RES);
  ledcAttachPin(SERVO_PIN, 2);

}
  /************************************************
 *          STEER
 ***********************************************/

  void steer(uint16_t pwmValue) {
    ledcWrite(2, pwmValue);
  }

  /************************************************
 *          CENTER
 ***********************************************/

  void steerCenter() {
    steer(SERVO_CENTER);
  }

  /************************************************
 *          LEFT
 ***********************************************/

  void steerLeft() {
    steer(SERVO_LEFT);
  }

  /************************************************
 *          RIGHT
 ***********************************************/

  void steerRight() {
    steer(SERVO_RIGHT);
  }

