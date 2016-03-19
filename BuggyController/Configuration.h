
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Arduino.h"
#define DEBUG true
#define DEBUG_PRINT_TIME 500
#define SENSOR_READ_TIME 10

// This determines the communication speed
#define BAUDRATE 38400
#define DEBUG_BUFSIZ 512
#define DISP_BUFSIZ 100
#define SYSTEM_VCC 5.0
#define SYSTEM_ANALOGUE_MAX 1023
#define SYSTEM_PWM_MAX 255

#define ENABLE_SMOOTHING
// #define ENABLE_NORMALIZATION

#define BATTERIES 2

#if DEBUG

    #define MOTORS 2

    // #define IGNORE_THROTTLE
    // #define IGNORE_TEMPS 
    // #define IGNORE_BATTERY
    // #define IGNORE_CURRENTS
    // #define IGNORE_MODE
    // #if MOTORS < 1
    //     #define DEBUG_CURRENTS
    // #endif
    #define CALLIBRATE_SENSORS
    // #define DATA_LOGGING

#else
    
    #define MOTORS 2

#endif



// #define IGNORE_SYS_TEMP

/* Matt, uncomment this line to get it to work on your arduinoi */
// #define BOARD 0
// #define BOARD 1
#define BOARD 1

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
// #define MOTOR_0_MINTEMP 5.0
#define MOTOR_0_MINTEMP -1000.0
#define MOTOR_1_MINTEMP 5.0

// The tempterature at which the throttle should be regulated in order to bring the temperature down
#define ONBOARD_REGTEMP 31.0
// #define ONBOARD_REGTEMP 70.0
// #define MOTOR_0_REGTEMP 31.0
#define MOTOR_0_REGTEMP 10000.0
// #define MOTOR_0_REGTEMP 70.0
#define MOTOR_1_REGTEMP 31.0
// #define MOTOR_1_REGTEMP 70.0

// When temperature exceeds max temp, your motor will be switched off.
// This feature exists to protect your motor from overheating accidentally, but *NOT* from thermistor short/failure!
// You should use MINTEMP for thermistor short/failure protection.
#define ONBOARD_MAXTEMP 32.0
#define MOTOR_0_MAXTEMP 100000.0
#define MOTOR_1_MAXTEMP 32.0
// #define ONBOARD_MAXTEMP 80.0
// #define MOTOR_0_MAXTEMP 80.0
// #define MOTOR_1_MAXTEMP 80.0

//===========================================================================
//============================= Current Settings ============================
//===========================================================================
//

// #define CURRENT_SENSOR_0 1
// #define CURRENT_SENSOR_1 1

// #define DEBUG_REG_CURRENT 20
#define MOTOR_0_REG_CURRENT 30
#define MOTOR_1_REG_CURRENT 30

// #define DEBUG_MAX_CURRENT 25
#define MOTOR_0_MAX_CURRENT 35
#define MOTOR_1_MAX_CURRENT 35

//===========================================================================
//============================== Field Settings =============================
//===========================================================================
//

#define FIELD_OFF 0
#define FIELD_ON 255
#define FIELD_BOOST 127

#define FIELD_TARGET_VOLT 12.0


//===========================================================================
//============================= Battery Settings ============================
//===========================================================================
//
#define BATTERY_SENSOR_MULTIPLIER 6.0
#if BATTERIES == 1
    #define BATTERY_MIN 11.0
    #define BATTERY_MAX 14.5
#elif BATTERIES == 2
    #define BATTERY_MIN 21
    #define BATTERY_MAX 31
#endif


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
    #define BATTERY_VOLT_PIN A0
    #define THROTTLE_PIN A0
    #define ONBOARD_TEMP_PIN A0
    #define DEBUG_CURRENT_PIN A0
    #define VEHICLE_MODE_PIN 40

    #define MOTOR_FIELD_VOLT_PIN 2

    #define MOTOR_FIELD_PHASE_PIN 38

    #define MOTOR_0_TEMP_PIN A0
    #define MOTOR_0_ARM_SENSE_PIN A0
    #define MOTOR_0_ARM_VOLT_PIN 5


#endif

#ifdef DATA_LOGGING
    #define DEBUG_CELL_WIDTH 6
    #define FLOAT_WIDTH 6
    #define FLOAT_PREC 2
    #define DEBUG_DELIMETER ","
#else
    #define DEBUG_CELL_WIDTH 3
    #define FLOAT_WIDTH 3
    #define FLOAT_PREC 2
    #define DEBUG_DELIMETER "|"
#endif

#if DEBUG_CELL_WIDTH == 3
    #define DEBUG_CELL_FMT_D "%3d"
    #define DEBUG_CELL_FMT_LT1 ".%02d"
#else
    #define DEBUG_CELL_FMT_D "%6d"
    #define DEBUG_CELL_FMT_LT1 ".%05d"
#endif

#endif
