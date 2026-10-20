#include "ParkingController.h"

#include "Config.h"
#include "Encoder.h"
#include "LcdDisplay.h"
#include "Motor.h"
#include "Steering.h"
#include "Ultrasonic.h"

enum ParkingStep {
  IDLE,
  STEP_BACK_RIGHT,
  STEP_MID_BACK_STRAIGHT,
  STEP_BACK_LEFT,
  STEP_FORWARD_RIGHT,
  STEP_FINAL_BACK_CENTER,
  PERP_BACK_RIGHT,
  PERP_FORWARD_LEFT_STRAIGHTEN,
  PERP_BACK_STRAIGHT,
  PERP_FINAL_CENTER,
  FINISHED
};

static ParkingStep currentStep = IDLE;
static unsigned long stepStartPulseCount = 0;
static float perpendicularTurnDistanceCm = 0.0f;
static const char* currentParkingMode = "Parking";

static bool distanceIsUnsafe(float distance, float limit) {
  // -1 means an invalid/timeout measurement, not an obstacle.
  return distance > 0.0f && distance <= limit;
}

static bool parkingCollisionRisk() {
  return distanceIsUnsafe(getRearDistance(), REAR_SAFE_DISTANCE) ||
         distanceIsUnsafe(getFrontDistance(), FRONT_SAFE_DISTANCE);
}

static bool frontCollisionRisk() {
  return distanceIsUnsafe(getFrontDistance(), FRONT_SAFE_DISTANCE);
}

static bool rearCollisionRisk() {
  return distanceIsUnsafe(getRearDistance(), REAR_SAFE_DISTANCE);
}

static bool stepDistanceReached(float targetDistanceCm) {
  const unsigned long pulses = getEncoderPulseCount() - stepStartPulseCount;
  return encoderPulsesToCm(pulses) >= targetDistanceCm;
}

static void changeStep(ParkingStep nextStep, const char* message) {
  stopMotor();
  currentStep = nextStep;
  stepStartPulseCount = getEncoderPulseCount();
  Serial.println(message);
  lcdDisplayShowParkingStatus(currentParkingMode, message);
}

void parkingControllerInit() {
  currentStep = IDLE;
}

void startParallelParking() {
  currentParkingMode = "Parallel Park";
  currentStep = STEP_BACK_RIGHT;
  stepStartPulseCount = getEncoderPulseCount();

  Serial.println();
  Serial.println("===== PARALLEL PARKING =====");
  Serial.print("Right reverse turn: ");
  Serial.println(PARK_RIGHT_REVERSE_TURN_CM);
  Serial.println("Step 1: reverse right");
  lcdDisplayShowParkingStatus(currentParkingMode, "Back Right");
}

void startPerpendicularParking() {
  currentParkingMode = "Perp. Park";
  perpendicularTurnDistanceCm = PERP_RIGHT_REVERSE_TURN_CM;
  currentStep = PERP_BACK_RIGHT;
  stepStartPulseCount = getEncoderPulseCount();

  Serial.println();
  Serial.println("===== PERPENDICULAR PARKING =====");
  Serial.print("Right reverse turn distance: ");
  Serial.println(perpendicularTurnDistanceCm);
  Serial.println("Step 1: reverse right");
  lcdDisplayShowParkingStatus(currentParkingMode, "Back Right");
}

void cancelParallelParking() {
  stopMotor();
  steerCenter();
  currentStep = IDLE;
}

void parkingControllerUpdate() {
  switch (currentStep) {
    case IDLE:
      break;

    // The rear axle enters the parking space on a right arc.
    case STEP_BACK_RIGHT:
      steerRight();
      backward(PARKING_SPEED);

      if (parkingCollisionRisk()) {
        changeStep(FINISHED, "Parking stopped: obstacle too close");
      } else if (stepDistanceReached(PARK_RIGHT_REVERSE_TURN_CM)) {
        changeStep(STEP_MID_BACK_STRAIGHT, "Step 2: reverse straight");
      }
      break;

    // Settle the vehicle before the forward steering correction.
    case STEP_MID_BACK_STRAIGHT:
      steerCenter();
      backward(PARKING_SPEED);

      if (parkingCollisionRisk()) {
        changeStep(FINISHED, "Parking stopped: obstacle too close");
      } else if (stepDistanceReached(PARK_MID_REVERSE_CM)) {
        changeStep(STEP_BACK_LEFT, "Step 3: reverse left");
      }
      break;

    // Counter-steer while reversing to bring the vehicle closer to parallel.
    case STEP_BACK_LEFT:
      steerLeft();
      backward(PARKING_SPEED);

      if (parkingCollisionRisk()) {
        changeStep(FINISHED, "Parking stopped: obstacle too close");
      } else if (stepDistanceReached(PARK_LEFT_REVERSE_TURN_CM)) {
        changeStep(STEP_FORWARD_RIGHT, "Step 4: forward right");
      }
      break;

    // Counter the parking angle by moving forward with right steering.
    case STEP_FORWARD_RIGHT:
      steerRight();
      forward(PARKING_SPEED);

      if (frontCollisionRisk()) {
        changeStep(FINISHED, "Parking stopped: front obstacle too close");
      } else if (stepDistanceReached(PARK_FORWARD_RIGHT_TURN_CM)) {
        changeStep(STEP_FINAL_BACK_CENTER, "Step 5: reverse to centre");
      }
      break;

    // Finish by reversing straight to centre the vehicle inside the space.
    case STEP_FINAL_BACK_CENTER:
      steerCenter();
      backward(PARKING_SPEED);

      if (parkingCollisionRisk()) {
        changeStep(FINISHED, "Parking stopped: obstacle too close");
      } else if (stepDistanceReached(PARK_FINAL_CENTER_REVERSE_CM)) {
        changeStep(FINISHED, "Parking finished");
      }
      break;

    // Enter a right-side perpendicular bay by reversing on a right arc.
    case PERP_BACK_RIGHT:
      steerRight();
      backward(PARKING_SPEED);

      if (rearCollisionRisk()) {
        changeStep(FINISHED, "Perpendicular parking stopped: rear obstacle");
      } else if (stepDistanceReached(perpendicularTurnDistanceCm)) {
        changeStep(PERP_FORWARD_LEFT_STRAIGHTEN,
                   "Step 2: forward left to straighten");
      }
      break;

    // Counter-steer forward to reduce the wide reverse-right turning angle.
    case PERP_FORWARD_LEFT_STRAIGHTEN:
      steerLeft();
      forward(PARKING_SPEED);

      if (frontCollisionRisk()) {
        changeStep(FINISHED, "Perpendicular parking stopped: front obstacle");
      } else if (stepDistanceReached(PERP_FORWARD_LEFT_STRAIGHTEN_CM)) {
        changeStep(PERP_BACK_STRAIGHT, "Step 3: reverse straight into bay");
      }
      break;

    // Finish entering the bay with the vehicle pointing perpendicular to lane.
    case PERP_BACK_STRAIGHT:
      steerCenter();
      backward(PARKING_SPEED);

      if (rearCollisionRisk()) {
        changeStep(PERP_FINAL_CENTER, "Step 4: forward to centre");
      } else if (stepDistanceReached(PERP_REVERSE_IN_DISTANCE_CM)) {
        changeStep(PERP_FINAL_CENTER, "Step 4: forward to centre");
      }
      break;

    // Return slightly from the rear boundary to centre the vehicle in depth.
    case PERP_FINAL_CENTER:
      steerCenter();
      forward(PARKING_SPEED);

      if (frontCollisionRisk()) {
        changeStep(FINISHED, "Perpendicular parking: front obstacle");
      } else if (stepDistanceReached(PERP_FINAL_CENTER_FORWARD_CM)) {
        changeStep(FINISHED, "Perpendicular parking finished");
      }
      break;

    case FINISHED:
      stopMotor();
      steerCenter();
      break;
  }
}

bool parkingFinished() {
  return currentStep == FINISHED;
}
