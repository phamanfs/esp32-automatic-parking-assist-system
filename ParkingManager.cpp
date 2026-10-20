#include "ParkingManager.h"

#include "Config.h"
#include "Encoder.h"
#include "LcdDisplay.h"
#include "Motor.h"
#include "ParkingController.h"
#include "ParkingDetector.h"
#include "Steering.h"
#include "Ultrasonic.h"

static unsigned long moveStartPulseCount = 0;

enum ParkingMode {
  PARALLEL_MODE,
  PERPENDICULAR_MODE
};

static ParkingMode parkingMode = PARALLEL_MODE;

enum ParkingState {
  MANUAL,
  SEARCH_SPACE,
  MOVE_TO_START_SLOWDOWN,
  PARKING,
  FINISHED
};

static ParkingState parkingState = MANUAL;

static bool frontObstacleTooClose() {
  float distance = getFrontDistance();
  return distance > 0.0f && distance <= FRONT_SAFE_DISTANCE;
}

static float moveToStartDistanceCm() {
  return parkingMode == PARALLEL_MODE ?
    MOVE_TO_START_DISTANCE_CM : PERP_MOVE_TO_REVERSE_POINT_CM;
}

static bool moveToStartReached() {
  const unsigned long pulses = getEncoderPulseCount() - moveStartPulseCount;
  return encoderPulsesToCm(pulses) >= moveToStartDistanceCm();
}

static float moveToStartRemainingCm() {
  const unsigned long pulses = getEncoderPulseCount() - moveStartPulseCount;
  const float remaining = moveToStartDistanceCm() - encoderPulsesToCm(pulses);
  return remaining > 0.0f ? remaining : 0.0f;
}

static uint8_t moveToStartSlowSpeed() {
  const float targetDistance = moveToStartDistanceCm();
  if (targetDistance <= 0.0f) {
    return MOVE_STOP_MIN_SPEED;
  }

  const float ratio = moveToStartRemainingCm() / targetDistance;
  const float speed = MOVE_STOP_MIN_SPEED +
    (MOVE_STOP_START_SPEED - MOVE_STOP_MIN_SPEED) * ratio;
  return constrain((uint8_t)speed, MOVE_STOP_MIN_SPEED, MOVE_STOP_START_SPEED);
}

static void beginSelectedParking() {
  stopMotor();
  if (parkingMode == PARALLEL_MODE) {
    startParallelParking();
  } else {
    startPerpendicularParking();
  }
  ultrasonicSetSamplingMode(ULTRASONIC_MODE_PARKING);
  parkingState = PARKING;
}

void parkingManagerInit() {
  parkingState = MANUAL;
  parkingDetectorInit();
  parkingControllerInit();
  ultrasonicSetSamplingMode(ULTRASONIC_MODE_ALL);
}

void startAutoParking() {
  if (parkingState != MANUAL) {
    return;
  }

  parkingMode = PARALLEL_MODE;
  startParkingDetection(MIN_PARKING_SPACE);
  ultrasonicSetSamplingMode(ULTRASONIC_MODE_SEARCH);
  steerCenter();
  parkingState = SEARCH_SPACE;
  Serial.println();
  Serial.println("===== AUTO PARKING START =====");
  lcdDisplayShow("Parallel Park", "Searching...");
}

void startAutoPerpendicularParking() {
  if (parkingState != MANUAL) {
    return;
  }

  parkingMode = PERPENDICULAR_MODE;
  startParkingDetection(PERP_MIN_PARKING_SPACE_CM);
  ultrasonicSetSamplingMode(ULTRASONIC_MODE_SEARCH);
  steerCenter();
  parkingState = SEARCH_SPACE;
  Serial.println();
  Serial.println("===== AUTO PERPENDICULAR PARKING START =====");
  lcdDisplayShow("Perp. Park", "Searching...");
}

void cancelAutoParking() {
  cancelParallelParking();
  stopMotor();
  steerCenter();
  ultrasonicSetSamplingMode(ULTRASONIC_MODE_ALL);
  parkingState = MANUAL;
  Serial.println("Auto parking cancelled");
  lcdDisplayShow("Parking", "Cancelled");
}

void parkingManagerUpdate() {
  switch (parkingState) {
    case MANUAL:
      break;

    case SEARCH_SPACE:
      if (frontObstacleTooClose()) {
        cancelAutoParking();
        Serial.println("Auto parking stopped: front obstacle");
        break;
      }

      steerCenter();
      parkingDetectorUpdate();
      forward(SEARCH_SPEED);

      if (parkingSpaceFound()) {
        moveStartPulseCount = getEncoderPulseCount();
        parkingState = MOVE_TO_START_SLOWDOWN;
        Serial.println("Parking space found");
        lcdDisplayShow("Space Found", "Slowing down...");
      }
      break;

    case MOVE_TO_START_SLOWDOWN:
      if (frontObstacleTooClose()) {
        cancelAutoParking();
        Serial.println("Auto parking stopped: front obstacle");
        break;
      }

      steerCenter();
      if (moveToStartReached()) {
        beginSelectedParking();
      } else {
        forward(moveToStartSlowSpeed());
      }
      break;

    case PARKING:
      parkingControllerUpdate();
      if (parkingFinished()) {
        parkingState = FINISHED;
        Serial.println("Parking completed");
      }
      break;

    case FINISHED:
      stopMotor();
      steerCenter();
      ultrasonicSetSamplingMode(ULTRASONIC_MODE_ALL);
      parkingState = MANUAL;
      lcdDisplayShow("Parking", "Completed");
      break;
  }
}

bool autoParkingRunning() {
  return parkingState != MANUAL;
}
