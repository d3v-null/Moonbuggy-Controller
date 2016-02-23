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
#include "statusEnums.h"

// Safety Stuff
safetyStatusType safetyStatus = S_SAFE; // Safety status of system, starts off as safe

// throttleStatusType throttleStatus;

motorModeType vehicleMode = M_NEUTRAL; // Mode of all motors, starts off as Neutral

double throttleNormalized; // The normalized value of the throttle (0.0 -> 1.0)

int killSwitch;  // The value of the killSwitch sensor

int modeSwitch; // The value of the modeSwitch sensor

MotorController motorControllers[MOTORS];


throttleStatusType getThrottleStatus(){
    throttleStatusType throttleStatus = TH_BOOST;
    int statuses[][2] = {
        {THROTTLE_THRESHOLD_ZERO, TH_ZERO},
        {THROTTLE_THRESHOLD_BOOST, TH_NORMAL}
    };
    int statusLen = sizeof(statuses) / sizeof(statuses[0]);
    int i;
    for( i=0; i<statusLen; i++) {
        if(throttleNormalized > statuses[i][0]){
            throttleStatus = (throttleStatusType)(statuses[i][1]);
        }
    }
    return throttleStatus;
}

void readInputs() {
    // Store the normalized value of the throttle
    throttleNormalized = map(analogRead(THROTTLE_PIN), THROTTLE_MIN, THROTTLE_MAX, 0.0, 1.0);
    // Process the status of the throttle
    // throttleStatus = 
    // Store the value of killSwitch in killSwitch;
    killSwitch = digitalRead(KILLSWITCH_PIN);

    modeSwitch = digitalRead(VEHICLE_MODE_PIN);

    int i;
    for(i=0; i<MOTORS; i++){
        motorControllers[i].readInputs();
    }
}

void shutdown() {
    Serial.println("Shutting Down");

    vehicleMode = M_NEUTRAL;
    int i;
    for(i=0; i<MOTORS; i++){
        motorControllers[i].shutdown();
    }   
}

/**
 * If system thinks that it is safe, performs a check of safety parameters, 
 * updates variable safetyStatus
 */
void safetyCheck() {
    boolean shouldTerminate = false;
    switch (safetyStatus) {
        case S_SAFE:
            int i;
            for(i=0; i<MOTORS; i++){
                if(! IGNORE_TEMPS ){
                    switch (motorControllers[i].getTempStatus()) {
                        case T_COLD:
                          Serial.print("Failed temp check on MOTOR: T_COLD, ");
                          Serial.println(i);
                          shouldTerminate = true;
                          break;
                        case T_HOT:
                          Serial.print("Failed temp check on MOTOR: T_HOT, ");
                          Serial.println(i);
                          shouldTerminate = true;
                          break;
                        default:
                          break;
                    }
                    if( shouldTerminate ){ break; }
                }
                if(! IGNORE_CURRENTS and motorControllers[i].getArmStatus() == A_HIGH){
                    Serial.print("Failed current check on MOTOR: A_HIGH, ");
                    Serial.println(i);
                    shouldTerminate = true;
                    break;
                }
            }
            if(!shouldTerminate and killSwitch == HIGH){
                Serial.println("Killswitch Engaged");
                shouldTerminate = true;
            }
            // checks killSwitch value
            // checks voltages
            // checks temperatures
            // updates safetyStatus if necessary
            // terminates system if necessary
            if(shouldTerminate){
                safetyStatus = S_TERMINATING;
                shutdown();
                safetyStatus = S_TERMINATED;
            }
            break;
        case S_TERMINATING:
            // Should not be terminating
            shutdown();
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
        motorControllers[0].setTempBounds(
            TEMP_SENSOR_0,
            MOTOR_0_MINTEMP,
            MOTOR_0_REGTEMP,
            MOTOR_0_MAXTEMP,
            IGNORE_TEMPS
        );
        motorControllers[0].setArmBounds(
            CURRENT_SENSOR_0,
            MOTOR_0_REG_CURRENT,
            MOTOR_0_MAX_CURRENT,
            IGNORE_CURRENTS
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
        motorControllers[1].setTempBounds(
            TEMP_SENSOR_1,
            MOTOR_1_MINTEMP,
            MOTOR_1_REGTEMP,
            MOTOR_1_MAXTEMP,
            IGNORE_TEMPS
        );
        motorControllers[1].setArmBounds(
            CURRENT_SENSOR_1,
            MOTOR_1_REG_CURRENT,
            MOTOR_1_MAX_CURRENT,
            IGNORE_CURRENTS
        );
        motorControllers[1].initPins();
    }

    throttleNormalized = 0.0;
    // throttleStatus = TH_ZERO;
    vehicleMode = M_NEUTRAL;
}

char* digitalStatus(int digitalValue){
    return "off";
}

void printDebugInfo(){
    // |KSW|VMP|THS|THR|SSS|SMD - system
    // TPN|ASN|AVN|FVN|FPN| - per motor

}

void loop() {
    readInputs();
    safetyCheck();
    if(safetyStatus == S_SAFE){
        // set vehicle mode
        switch (getThrottleStatus()) {
            case TH_ZERO:
                if(vehicleMode != M_NEUTRAL){
                    shutdown();                   
                }
              break;
            case TH_NORMAL:
                switch (modeSwitch) {
                    case LOW:
                        vehicleMode = M_FORWARD;
                        break;
                    case HIGH:
                        vehicleMode = M_REVERSE;
                        break;
                    default:
                        break;
                }
                break;
            case TH_BOOST:
                switch (modeSwitch) {
                    case LOW:
                        vehicleMode = M_FORWARD_BOOST;
                        break;
                    case HIGH:
                        vehicleMode = M_REVERSE;
                        break;
                    default:
                        break;
                }
                break;
            default:
              break;
        };

        // set throttles
        int i;
        for(i=0; i<MOTORS; i++){
            motorControllers[i].setMotorMode(vehicleMode);
            motorControllers[i].setThrottle(throttleNormalized);
            motorControllers[i].updateOutputs();
        }
    }    
    if(DEBUG){
        printDebugInfo();
    }
}

