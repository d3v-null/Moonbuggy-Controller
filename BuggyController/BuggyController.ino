/*
    Shunt wound motor control firmware 

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Buggy Controller
 * This program monitors safety parameters and sends information to 
 * the motor controller objects
 */

#include "Configuration.h"
#include "MotorController.h"

// Safety Stuff
typedef enum {SAFE, TERMINATING, TERMINATED} safetyStatusType;
safetyStatusType safetyStatus = SAFE; // Safety status of system, starts off as safe
motorModeType vehicleMode = M_NEUTRAL; // Mode of all motors, starts off as Neutral

double throttleNormalized; // The normalized value of the throttle (0.0 -> 1.0)

int killSwitch;  // The value of the killswitch sensor

MotorController motorControllers[MOTORS];

void readInputs() {
    // Store the value of the throttle in throttleRaw
    throttleNormalized = map(analogRead(THROTTLE_PIN), THROTTLE_MIN, THROTTLE_MAX, 0.0, 1.0);

    // Store the value of killswitch in killSwitch;

}

/**
 * If system thinks that it is safe, performs a check of safety parameters, 
 * updates variable safetyStatus
 */
void safetyCheck() {
    switch (safetyStatus) {
        case SAFE:
            // checks killswitch value
            // checks voltages
            // checks temperatures
            // updates safetyStatus if necessary
            // terminates system if necessary
            break;
        case TERMINATING:
            // Should not be terminating
            break;
        default:
            break;
    }
}

void setup() {
    Serial.begin(BAUDRATE);
    Serial.println("Initializing");

    //Set pinmodes
    pinMode(THROTTLE_PIN, INPUT);
    pinMode(KILLSWITCH_PIN, INPUT);
    pinMode(VEHICLE_MODE_PIN, INPUT);

    if(MOTORS > 0){
        Serial.println("Initializing Motor 1");
        motorControllers[0].setPins(
            MOTOR_0_TEMP_PIN,
            MOTOR_0_ARM_SENSE_PIN,
            MOTOR_0_ARM_VOLT_PIN,
            MOTOR_0_FIELD_VOLT_PIN,
            MOTOR_0_FIELD_PHASE_PIN
        );
        motorControllers[0].initPins();
    }
    if(MOTORS > 1){
        Serial.println("Initializing Motor 2");
        motorControllers[1].setPins(
            MOTOR_1_TEMP_PIN,
            MOTOR_1_ARM_SENSE_PIN,
            MOTOR_1_ARM_VOLT_PIN,
            MOTOR_1_FIELD_VOLT_PIN,
            MOTOR_1_FIELD_PHASE_PIN
        );
        motorControllers[1].initPins();
    }
}

void loop() {
    safetyCheck();
    if(safetyStatus == SAFE){

    }    

}