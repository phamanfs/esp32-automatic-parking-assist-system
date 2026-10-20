#include "ParkingDetector.h"

#include "Config.h"
#include "Encoder.h"
#include "Ultrasonic.h"

enum DetectorState {
  WAIT_CAR,
  FOUND_GAP,
  SPACE_FOUND
};

static DetectorState detectorState;

static bool spaceFound = false;

static unsigned long gapTime = 0;

/************************************************
 *          GAP INFORMATION
 ***********************************************/

static unsigned long gapStartTime = 0;

static unsigned long gapEndTime = 0;

static float gapLength = 0;

static float minimumSpaceCm = MIN_PARKING_SPACE;

static unsigned long gapStartPulseCount = 0;

static unsigned long gapPulses = 0;

void parkingDetectorInit() {
  detectorState = WAIT_CAR;

  spaceFound = false;
}

void resetParkingDetector() {
  detectorState = WAIT_CAR;

  spaceFound = false;
}

void startParkingDetection(float requiredMinimumSpaceCm) {
  resetParkingDetector();
  minimumSpaceCm = requiredMinimumSpaceCm;
}

bool parkingSpaceFound() {
  return spaceFound;
}

void parkingDetectorUpdate() {
  float rightDistance = getRightDistance();

  // The ultrasonic filter has not received enough valid samples yet, or the
  // latest echo timed out. Keep the current detector state unchanged.
  if (rightDistance <= 0.0f) {
    return;
  }

  Serial.print("Right = ");
  Serial.print(rightDistance);
  Serial.print(" | State = ");

  switch (detectorState) {
    case WAIT_CAR:

      Serial.println("WAIT_CAR");

      if (rightDistance > GAP_DISTANCE_THRESHOLD) {
        gapStartTime = millis();
        gapStartPulseCount = getEncoderPulseCount();

        detectorState = FOUND_GAP;

        Serial.println("Gap Started");
      }

      break;

    case FOUND_GAP:

      Serial.println("FOUND_GAP");

      if (rightDistance <= GAP_DISTANCE_THRESHOLD) {
        detectorState = SPACE_FOUND;

        gapEndTime = millis();
        gapTime = gapEndTime - gapStartTime;  // Diagnostic only.
        gapPulses = getEncoderPulseCount() - gapStartPulseCount;
        gapLength = encoderPulsesToCm(gapPulses);

        Serial.print("Gap Length = ");
        Serial.print(gapLength);
        Serial.println(" cm");

        Serial.print("Gap Pulses = ");
        Serial.println(gapPulses);

        if (gapLength >= minimumSpaceCm) {
          Serial.print("Gap Time = ");
          Serial.print(gapTime);
          Serial.println(" ms");

          spaceFound = true;
          detectorState = SPACE_FOUND;

          Serial.println("Parking Space Found");
        } else {
          detectorState = WAIT_CAR;

          Serial.println("Gap Too Small");
        }

        if (spaceFound) {
          Serial.println(">>> PARKING SPACE FOUND");
        }
      }

      break;

    case SPACE_FOUND:

      Serial.println("SPACE_FOUND");

      break;
  }
}

float getGapLength() {
  return gapLength;
}

unsigned long getGapTime()
{
    return gapTime;
}
