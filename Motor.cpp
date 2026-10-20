#include "Motor.h"
#include "Pins.h"
#include "Config.h"

/************************************************
 *              PRIVATE VARIABLE
 ***********************************************/

static uint8_t currentSpeed = MOTOR_SPEED;

/************************************************
 *              INITIALIZE
 ***********************************************/

void motorInit() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // PWM ENA
  ledcSetup(0, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  ledcAttachPin(ENA, 0);

  // PWM ENB
  ledcSetup(1, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  ledcAttachPin(ENB, 1);

  stopMotor();

  setMotorSpeed(currentSpeed);
}

/************************************************
 *              SET SPEED
 ***********************************************/

void setMotorSpeed(uint8_t speed) {
  currentSpeed = constrain(speed, 0, 255);

  ledcWrite(0, currentSpeed);

  ledcWrite(1, currentSpeed);
}

/************************************************
 *              FORWARD
 ***********************************************/

void forward(uint8_t speed) {
  setMotorSpeed(speed);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

/************************************************
 *              BACKWARD
 ***********************************************/

void backward(uint8_t speed) {
  setMotorSpeed(speed);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

/************************************************
 *              STOP
 ***********************************************/

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}