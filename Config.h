#ifndef CONFIG_H
#define CONFIG_H

/************************************************
 *              MOTOR CONFIG
 ***********************************************/

#define MOTOR_SPEED 180   // Manual Speed
#define SEARCH_SPEED 70  // Tìm chỗ đỗ
#define PARKING_SPEED 70  // Đỗ xe (rất chậm)

#define MOVE_TO_START_DISTANCE_CM  5.0f

// Start slowing down as soon as a parking space is found. This reduces sudden
// braking that can shift the rear of the vehicle sideways.
#define MOVE_STOP_START_SPEED        60
#define MOVE_STOP_MIN_SPEED          35

// Perpendicular parking configuration (parking bay is on the right).
#define PERP_MIN_PARKING_SPACE_CM       (CAR_WIDTH + 4.0f)
#define PERP_MOVE_TO_REVERSE_POINT_CM   4.0f
#define PERP_RIGHT_REVERSE_TURN_CM      31.8f
#define PERP_FORWARD_LEFT_STRAIGHTEN_CM  26.00f
#define PERP_REVERSE_IN_DISTANCE_CM     40.0f
#define PERP_FINAL_CENTER_FORWARD_CM    5.0f

// Kich thuoc mo hinh (cm)
#define CAR_LENGTH             27.0f
#define CAR_WIDTH              14.0f
#define CAR_WHEELBASE          15.0f

// Khoang cach an toan voi xe truoc/sau khi do (cm)
#define FRONT_SAFE_DISTANCE    10.0f
#define REAR_SAFE_DISTANCE     8.0f

// PWM 4300/5700 are the mechanical full-lock positions. This is the actual
// front-wheel angle at those positions and must be measured on the model.
#define MAX_WHEEL_ANGLE_DEG      30.0f

// Parking geometry is now controlled by encoder distance. PARKING_SPEED sets
// only the motor PWM used while driving each manoeuvre phase.

// Required sideways movement of the vehicle centre while entering a space.
#define PARKING_LATERAL_SHIFT    (CAR_WIDTH + 2.0f)

// Parallel parking manoeuvre distances, measured by encoder.
#define PARK_RIGHT_REVERSE_TURN_CM  27.0f
#define PARK_MID_REVERSE_CM          5.0f
#define PARK_LEFT_REVERSE_TURN_CM    20.0f
#define PARK_FORWARD_RIGHT_TURN_CM  8.0f
#define PARK_FINAL_CENTER_REVERSE_CM 5.0f

/************************************************
 *              SERVO CONFIG
 ***********************************************/

#define SERVO_CENTER 5010
#define SERVO_LEFT 5700
#define SERVO_RIGHT 4300

/************************************************
 *              PWM CONFIG
 ***********************************************/

// Servo

#define SERVO_PWM_FREQ 50
#define SERVO_PWM_RES 16


// Motor

#define MOTOR_PWM_FREQ 1000
#define MOTOR_PWM_RES 8

/************************************************
 *              BLUETOOTH
 ***********************************************/

#define BLUETOOTH_NAME "ESP32_AutoParking"

/************************************************
 *                    LCD
 ***********************************************/

#define LCD_I2C_ADDRESS  0x27
#define LCD_COLUMNS      20
#define LCD_ROWS         4

/************************************************
 *          PARKING DETECTOR
 ***********************************************/

#define GAP_DISTANCE_THRESHOLD 10.0f  // cm

/************************************************
 *      GAP DETECTION
 ***********************************************/

// Calibrate this once from a known travel distance. For example, 250 pulses
// over 100 cm means ENCODER_PULSES_PER_CM is 2.5f.
#define ENCODER_PULSES_PER_CM    1.00f

// Khoảng trống tối thiểu để đỗ
#define MIN_PARKING_SPACE        (CAR_LENGTH + FRONT_SAFE_DISTANCE + REAR_SAFE_DISTANCE)

/************************************************
 *          GAP DETECTION
 ***********************************************/

#endif
