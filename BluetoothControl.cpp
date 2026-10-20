#include "BluetoothControl.h"

#include <BluetoothSerial.h>

#include "Config.h"
#include "Motor.h"
#include "ParkingManager.h"
#include "Steering.h"

BluetoothSerial SerialBT;

void bluetoothInit() {
  SerialBT.begin(BLUETOOTH_NAME);
  Serial.println("Bluetooth Started");
  Serial.print("Device: ");
  Serial.println(BLUETOOTH_NAME);
}

static void executeCommand(char cmd) {
  // The automatic controller owns motor and steering until the user sends Stop.
  if (autoParkingRunning() && cmd != 'U') {
    return;
  }

  switch (cmd) {
    case 'F':
      steerCenter();
      forward(MOTOR_SPEED);
      break;
    case 'B':
      steerCenter();
      backward(MOTOR_SPEED);
      break;
    case 'L':
      steerLeft();
      stopMotor();
      break;
    case 'R':
      steerRight();
      stopMotor();
      break;
    case 'G':
      steerLeft();
      forward(MOTOR_SPEED);
      break;
    case 'I':
      steerRight();
      forward(MOTOR_SPEED);
      break;
    case 'H':
      steerLeft();
      backward(MOTOR_SPEED);
      break;
    case 'J':
      steerRight();
      backward(MOTOR_SPEED);
      break;
    case 'S':
      stopMotor();
      steerCenter();
      break;
    case 'V':
      startAutoParking();
      break;
    case 'X':
      startAutoPerpendicularParking();
      break;
    case 'U':
      cancelAutoParking();
      break;
  }
}

void processBluetooth() {
  while (SerialBT.available()) {
    char cmd = SerialBT.read();
    Serial.print("Received: ");
    Serial.println(cmd);
    executeCommand(cmd);
  }

}
