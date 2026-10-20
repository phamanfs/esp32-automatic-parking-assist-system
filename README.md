# ESP32 Automatic Parking Assist System

Firmware for an ESP32-based scale vehicle that supports Bluetooth manual driving, automatic parallel parking, and automatic perpendicular parking.

> This repository contains the completed firmware and supporting project documentation. The automatic manoeuvres are calibrated for the physical prototype; values such as encoder calibration, steering PWM positions, vehicle dimensions, and safety distances must be measured again before use with a different vehicle.

## Features

- Bluetooth Classic manual control.
- Four ultrasonic distance sensors: front, rear, left, and right.
- Encoder-based measurement of parking-space and manoeuvre distances.
- Automatic parking-space detection on the right side of the vehicle.
- Automatic parallel and perpendicular parking state machines.
- Front and rear collision-stop checks during automatic manoeuvres.
- 20x4 I2C LCD status and distance display.

## System overview

```text
Smartphone --Bluetooth--> ESP32 <--ultrasonic sensors / encoder
                            |--PWM--> L298N --> two DC drive motors
                            |--PWM--> SG90 steering servo
                            `--I2C--> 20x4 LCD
```

The main sketch initializes every module and repeatedly samples ultrasonic sensors, refreshes the LCD, processes Bluetooth commands, and updates the parking state machine.

## Hardware

| Component | Role |
| --- | --- |
| ESP32 DevKit | Main controller |
| 4 ultrasonic sensors | Front, rear, left, and right obstacle sensing |
| L298N | Dual DC-motor driver |
| 2 DC motors | Vehicle propulsion |
| SG90 servo | Front-wheel steering |
| Wheel encoder | Travel-distance feedback |
| 20x4 I2C LCD | Status and distance display |
| 3S 18650 battery pack and XL4015 buck converter | Prototype power system |

## Pin mapping

| Device | ESP32 pins |
| --- | --- |
| L298N | ENA 32, ENB 33, IN1 25, IN2 26, IN3 27, IN4 14 |
| SG90 servo | 13 |
| LCD I2C | SDA 21, SCL 22 |
| Front ultrasonic sensor | TRIG 4, ECHO 16 |
| Rear ultrasonic sensor | TRIG 17, ECHO 5 |
| Left ultrasonic sensor | TRIG 23, ECHO 15 |
| Right ultrasonic sensor | TRIG 18, ECHO 19 |
| Encoder | 34 |

The firmware in `Pins.h` is the authoritative pin mapping. The label for the right ultrasonic sensor's Echo pin in `docs/sodo.png` is outdated; use GPIO19, not GPIO18.

## Software requirements

- Arduino IDE with ESP32 board support installed.
- An ESP32 board selected in the IDE.
- The `LiquidCrystal_I2C` library installed.

`BluetoothSerial`, `Wire`, PWM (`ledc*`), interrupts, and the remaining Arduino APIs used by the project are provided by the ESP32 Arduino core.

## Build and upload

1. Open `ESP32_AutomaticParkingAssitSystem.ino` in Arduino IDE.
2. Select the ESP32 board and serial port that match your hardware.
3. Install `LiquidCrystal_I2C` if it is not already available.
4. Review and calibrate `Config.h` and `Pins.h` for the actual prototype before uploading.
5. Compile and upload from Arduino IDE.
6. Pair a phone with the Bluetooth Classic device `ESP32_AutoParking`.

The firmware has not been compiled in this repository environment because no Arduino CLI, board package, or hardware configuration is available here. Upload and behaviour on real hardware must be verified on the target prototype.

## Bluetooth commands

| Command | Action |
| --- | --- |
| `F`, `B` | Move forward, move backward |
| `L`, `R` | Steer left, steer right while stopped |
| `G`, `I` | Move forward-left, forward-right |
| `H`, `J` | Move backward-left, backward-right |
| `S` | Stop and center steering |
| `V` | Start automatic parallel parking |
| `X` | Start automatic perpendicular parking |
| `U` | Cancel automatic parking |

## Automatic parking flow

1. In automatic mode, the vehicle moves forward while the right ultrasonic sensor scans for an opening.
2. The encoder measures the opening length. A parallel-space opening must meet the configured minimum of 45 cm.
3. Once a suitable opening is found, the vehicle slows, moves to its configured starting point, and begins the selected manoeuvre.
4. Each manoeuvre step controls motor direction, steering position, and encoder travel distance.
5. During parking, front and rear distance checks stop the manoeuvre when an obstacle enters the configured safety threshold.

Parallel and perpendicular manoeuvre distances are defined in `Config.h`; the transition logic is implemented in `ParkingController.cpp`.

## Repository layout

```text
.
|- ESP32_AutomaticParkingAssitSystem.ino  Main Arduino sketch
|- Config.h                               Calibration and control constants
|- Pins.h                                 Hardware pin mapping
|- *Control / *Controller / *Manager      Bluetooth and parking logic modules
|- Ultrasonic.* / Encoder.*               Sensor and distance-feedback modules
|- Motor.* / Steering.* / LcdDisplay.*    Actuator and display modules
`- docs/                                  Project report and wiring diagram
```

## Documentation

- `docs/26SU_SE2041_Group4_IOT102_Report.pdf`: project report, prototype information, experiment description, and demo link.
- `docs/sodo.png`: prototype power and wiring diagram. See the right-sensor Echo correction in the pin-mapping section above.

## Known limitations

- The current controller uses fixed, prototype-specific distances and steering PWM values; it is not a general path-planning system.
- Encoder calibration is set to `1.00` pulse/cm by default and must be calibrated against a known travel distance.
- A timed-out ultrasonic reading is treated as invalid, not as an obstacle.
- Sensor collision checks during automatic parking are implemented for the front and rear directions; side readings are used primarily for space detection.
- This repository does not include automated tests or a reproducible command-line build configuration.

## License

No license has been selected yet. Do not assume permission to reuse, redistribute, or modify the source code, report, diagrams, or other assets until the project owners choose and add an appropriate license.
