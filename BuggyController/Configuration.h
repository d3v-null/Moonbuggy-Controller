
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Arduino.h"
#define DEBUG true
#define DEBUG_PRINT_DELAY 200

// This determines the communication speed
#define BAUDRATE 115200
#define DEBUG_BUFSIZ 255
#define DISP_BUFSIZ 100
#define SYSTEM_VCC 5.0
#define SYSTEM_ANALOGUE_MAX 1023

#define FLOAT_WIDTH 6
#define FLOAT_PREC 2

#if DEBUG

    #define MOTORS 0

    // #define IGNORE_THROTTLE
    // #define IGNORE_TEMPS 
    #define IGNORE_BATTERY
    // #define IGNORE_CURRENTS
    #define IGNORE_MODE
    // #if MOTORS < 1
    //     #define DEBUG_CURRENTS
    // #endif
    #define CALLIBRATE_SENSORS

#else
    
    #define MOTORS 2

#endif

/* Matt, uncomment this line to get it to work on your arduinoi */
// #define BOARD 0
// #define BOARD 1
#define BOARD 2

//===========================================================================
//============================ Threottle Settings ===========================
//===========================================================================

#define THROTTLE_RAW_MIN 180
#define THROTTLE_RAW_MAX 810
// #define THROTTLE_THRESHOLD_BOOST 0.90
#define THROTTLE_THRESHOLD_BOOST 1
#define THROTTLE_THRESHOLD_ZERO 0.05

//===========================================================================
//============================= Thermal Settings ============================
//===========================================================================

// #define TEMP_SENSOR_ONBOARD 1
// #define TEMP_SENSOR_0 1
// #define TEMP_SENSOR_1 1

// The minimal temperature defines the temperature below which the motor will not be enabled It is used
// to check that the wiring to the thermistor is not broken.
#define ONBOARD_MINTEMP 5.0
#define MOTOR_0_MINTEMP 5.0
#define MOTOR_1_MINTEMP 5.0

// The tempterature at which the throttle should be regulated in order to bring the temperature down
#define ONBOARD_REGTEMP 70.0
#define MOTOR_0_REGTEMP 70.0
#define MOTOR_1_REGTEMP 70.0

// When temperature exceeds max temp, your motor will be switched off.
// This feature exists to protect your motor from overheating accidentally, but *NOT* from thermistor short/failure!
// You should use MINTEMP for thermistor short/failure protection.
#define ONBOARD_MAXTEMP 80.0
#define MOTOR_0_MAXTEMP 80.0
#define MOTOR_1_MAXTEMP 80.0

//===========================================================================
//============================= Current Settings ============================
//===========================================================================
//

// #define CURRENT_SENSOR_0 1
// #define CURRENT_SENSOR_1 1

#define DEBUG_REG_CURRENT 30
#define MOTOR_0_REG_CURRENT 30
#define MOTOR_1_REG_CURRENT 30

#define DEBUG_MAX_CURRENT 40
#define MOTOR_0_MAX_CURRENT 40
#define MOTOR_1_MAX_CURRENT 40

//===========================================================================
//============================== Field Settings =============================
//===========================================================================
//

#define FIELD_OFF 0
#define FIELD_ON 255
#define FIELD_BOOST 127

//===========================================================================
//============================= Battery Settings ============================
//===========================================================================
//
#define BATTERY_SENSOR_MULTIPLIER 6.0
// #define BATTERY_MIN 22.8
// #define BATTERY_MAX 28.4
#define BATTERY_MIN 11.0
#define BATTERY_MAX 14.5

//===========================================================================
//=============================== Pin Settings ==============================
//===========================================================================
//

#if BOARD == 0

    #define KILLSWITCH_PIN 2
    #define THROTTLE_PIN A0
    #define ONBOARD_TEMP_PIN A1

    #define MOTOR_FIELD_VOLT_PIN 2

#endif


#if BOARD ==  1/*Arduino Mega w/ shield */

    #define KILLSWITCH_PIN 41
    #define THROTTLE_PIN A13    
    #define VEHICLE_MODE_PIN 40

    #define BATTERY_VOLT_PIN A2
    #define BRAKE_SENSE_PIN 42

    #define ONBOARD_TEMP_PIN A3

    #define MOTOR_FIELD_VOLT_PIN 2

    #define MOTOR_FIELD_PHASE_PIN 38

    #define MOTOR_0_TEMP_PIN A4
    #define MOTOR_0_ARM_SENSE_PIN A0
    #define MOTOR_0_ARM_VOLT_PIN 5

    #define MOTOR_1_TEMP_PIN A5
    #define MOTOR_1_ARM_SENSE_PIN A1
    #define MOTOR_1_ARM_VOLT_PIN 4

#endif

#if BOARD == 2 /*Arduino Uno w/ modified shield */

    #define KILLSWITCH_PIN 8
    #define BATTERY_VOLT_PIN A2
    #define THROTTLE_PIN A3
    #define ONBOARD_TEMP_PIN A4
    #define DEBUG_CURRENT_PIN A5
    #define VEHICLE_MODE_PIN 40

    #define MOTOR_FIELD_VOLT_PIN 2

    #define MOTOR_FIELD_PHASE_PIN 38

    #define MOTOR_0_TEMP_PIN A4
    #define MOTOR_0_ARM_SENSE_PIN A0
    #define MOTOR_0_ARM_VOLT_PIN 5


#endif


#endif
