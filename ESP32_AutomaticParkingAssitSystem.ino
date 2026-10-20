#include "Config.h"
#include "Pins.h"

#include "Motor.h"
#include "Steering.h"
#include "BluetoothControl.h"
#include "Encoder.h"
#include "LcdDisplay.h"
#include "Ultrasonic.h"
#include "ParkingManager.h"

void setup() {
  Serial.begin(115200);

  motorInit();

  steeringInit();

  encoderInit();

  lcdDisplayInit();

  ultrasonicInit();

  parkingManagerInit();

  bluetoothInit();

  Serial.println();

  Serial.println("===================================");

  Serial.println(" ESP32 AUTO PARKING SYSTEM ");

  Serial.println(" Version : 1.0");

  Serial.println("===================================");

  Serial.println();

  Serial.println("Mode : MANUAL");

  lcdDisplayShowManual();

  Serial.println();
}

void loop() {
  ultrasonicUpdate();
  lcdDisplayUpdateDistances();
  processBluetooth();
  parkingManagerUpdate();

}
