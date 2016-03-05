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

#include "BuggyController.h"

// Safety status of system, starts off as safe
safetyStatusType safetyStatus; 

// Mode of all motors, starts off as Neutral

motorModeType vehicleMode; 

// Current phase status
phaseType phaseStatus;

// sensors
ThrottleSensor throttleSensor;
#ifndef IGNORE_BATTERY
    BatterySensor batterySensor;
#endif
#ifndef IGNORE_TEMPS
    TemperatureSensor systemTempSensor;
#endif

// The value of the killSwitch sensor
int killSwitch; 

// The value of the modeSwitch sensor
int modeSwitch; 

// the value of the phase pin
int phaseVal;

// the value of the field 
int fieldVal;

// An array pointing to the motor controllers
MotorController motorControllers[MOTORS];

void setup() {
    if(DEBUG) Serial.begin(BAUDRATE);
    if(DEBUG) Serial.println("Initializing");
    delay(100);

    //Set pinmodes
    // pinMode(THROTTLE_PIN, INPUT);
    throttleSensor = ThrottleSensor();
    throttleSensor.setPins(THROTTLE_PIN);
    throttleSensor.initPins();
    throttleSensor.setInputConstraints(THROTTLE_RAW_MIN, THROTTLE_RAW_MAX);
    throttleSensor.setStatusBounds(THROTTLE_THRESHOLD_ZERO, THROTTLE_THRESHOLD_BOOST);

    #ifndef IGNORE_TEMPS

        systemTempSensor = TemperatureSensor();
        systemTempSensor.setPins(ONBOARD_TEMP_PIN);
        systemTempSensor.initPins();
        systemTempSensor.setSensorType(TEMP_SENSOR_ONBOARD);
        systemTempSensor.setStatusBounds(ONBOARD_MINTEMP, ONBOARD_REGTEMP, ONBOARD_MAXTEMP);

    #endif

    #ifndef IGNORE_BATTERY

        batterySensor = VoltageDividerSensor();
        batterySensor.setPins(BATTERY_VOLT_PIN);
        batterySensor.initPins();
        batterySensor.setMultiplier(BATTERY_SENSOR_MULTIPLIER);
        batterySensor.setStatusBounds(BATTERY_MIN, BATTERY_MAX);
    
    #endif


    pinMode(KILLSWITCH_PIN, INPUT);
    #ifndef IGNORE_MODE
        pinMode(VEHICLE_MODE_PIN, INPUT);
    #endif

    #if MOTORS > 0
        Serial.println("Initializing Fields");
        pinMode( MOTOR_FIELD_VOLT_H_PIN, OUTPUT );
        pinMode( MOTOR_FIELD_PHASE_PIN, OUTPUT);

        Serial.println("Initializing Motor 1");
        motorControllers[0].setPins(
            MOTOR_0_TEMP_PIN,
            MOTOR_0_ARM_SENSE_PIN,
            MOTOR_0_ARM_VOLT_PIN
        );
        motorControllers[0].setTempBounds(
            TEMP_SENSOR_0,
            MOTOR_0_MINTEMP,
            MOTOR_0_REGTEMP,
            MOTOR_0_MAXTEMP,
        );
        motorControllers[0].setArmBounds(
            CURRENT_SENSOR_0,
            MOTOR_0_REG_CURRENT,
            MOTOR_0_MAX_CURRENT,
        );
        motorControllers[0].initPins();
    #endif

    #if MOTORS > 1
        if(DEBUG) Serial.println("Initializing Motor 2");
        motorControllers[1].setPins(
            MOTOR_1_TEMP_PIN,
            MOTOR_1_ARM_SENSE_PIN,
            MOTOR_1_ARM_VOLT_PIN
        );
        motorControllers[1].setTempBounds(
            TEMP_SENSOR_1,
            MOTOR_1_MINTEMP,
            MOTOR_1_REGTEMP,
            MOTOR_1_MAXTEMP,
        );
        motorControllers[1].setArmBounds(
            CURRENT_SENSOR_1,
            MOTOR_1_REG_CURRENT,
            MOTOR_1_MAX_CURRENT,
        );
        motorControllers[1].initPins();
    #endif

    vehicleMode = M_NEUTRAL;
    safetyStatus = S_SAFE;
    modeSwitch = LOW;
    phaseStatus = P_FORWARD;
    phaseVal = HIGH;
    if(DEBUG) Serial.println("Finished Initializing"); delay(100);
}

void readInputs() {
    // Store the value of the kill switch in killSwitch;
    killSwitch = digitalRead(KILLSWITCH_PIN);

    throttleSensor.readInputs();
    #ifndef IGNORE_TEMPS
        systemTempSensor.readInputs();
    #endif
    #ifndef IGNORE_BATTERY
        batterySensor.readInputs();
    #endif
    #ifndef IGNORE_MODE
        modeSwitch = digitalRead(VEHICLE_MODE_PIN);
    #endif

    int i;
    for(i=0; i<MOTORS; i++){
        motorControllers[i].readInputs();
    }
}

void shutdown() {
    if(DEBUG) Serial.println("Shutting Down");

    vehicleMode = M_NEUTRAL;
    fieldVal = FIELD_OFF;
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
    // if(DEBUG) Serial.println("Starting safety check"); delay(100);
    boolean shouldTerminate = false;
    switch (safetyStatus) {
        case S_SAFE:
            if(!shouldTerminate and killSwitch == HIGH){
                if(DEBUG) Serial.println("Killswitch Engaged"); delay(100);
                shouldTerminate = true;
            }
            #ifndef IGNORE_TEMPS
                if(!shouldTerminate){
                    int i;
                    for(i=0; i<MOTORS; i++){
                        switch (motorControllers[i].getTempStatus()) {
                            case T_COLD:
                              if(DEBUG) Serial.print("Failed temp check on MOTOR: T_COLD, ");
                              if(DEBUG) Serial.println(i);
                              shouldTerminate = true;
                              break;
                            case T_HOT:
                              if(DEBUG) Serial.print("Failed temp check on MOTOR: T_HOT, ");
                              if(DEBUG) Serial.println(i);
                              shouldTerminate = true;
                              break;
                            default:
                              break;
                        }
                        if( shouldTerminate ){ break; }
                    }
                }
                if(!shouldTerminate){                    
                    switch(systemTempSensor.getStatus()){
                        case T_COLD:
                          if(DEBUG) Serial.println("Failed system temp check: T_COLD");
                          shouldTerminate = true;
                          break;
                        case T_HOT:
                          if(DEBUG) Serial.println("Failed system temp check: T_HOT");
                          shouldTerminate = true;
                          break;
                        default:
                          break;
                    }
                }
            #endif

            #ifndef IGNORE_CURRENTS
                if(!shouldTerminate){
                    int i;
                    for(i=0; i<MOTORS; i++){
    
                        if(! IGNORE_CURRENTS and motorControllers[i].getArmStatus() == C_HIGH){
                            if(DEBUG) Serial.print("Failed current check on MOTOR: C_HIGH, ");
                            if(DEBUG) Serial.println(i);
                            shouldTerminate = true;
                            break;
                        }
                    }
                }
            #endif

            #ifndef IGNORE_BATTERY
                if(!shouldTerminate){
                    switch(batterySensor.getStatus()){
                        case B_LOW:
                            if(DEBUG) Serial.println("Failed battery check: B_LOW");
                            shouldTerminate = true;
                            break;
                        case B_HIGH:
                            if(DEBUG) Serial.println("Failed battery check: B_HIGH");       
                            shouldTerminate = true;
                            break;
                        default:
                            break;
                    }
                }
            #endif

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
    // if(DEBUG) Serial.println("Finished safety check"); delay(100);
}

void setVehicleMode(motorModeType motorMode = M_NEUTRAL){
    // if(DEBUG) Serial.println("Setting Vehicle Mode"); delay(100);
    if(vehicleMode != motorMode){
        switch(motorMode){
            case M_REVERSE:
                fieldVal = FIELD_ON;
                setPhase(P_REVERSE);
                break;
            case M_FORWARD:
                fieldVal = FIELD_ON;
                setPhase(P_FORWARD);
                break;
            case M_FORWARD_BOOST:
                fieldVal = FIELD_BOOST;
                setPhase(P_FORWARD);
                break;
            case M_NEUTRAL:
            default:
                fieldVal = FIELD_OFF;
                break;
        }
        vehicleMode = motorMode;
        int i;
        for(i=0; i<MOTORS; i++){
            motorControllers[i].setMotorMode(vehicleMode);
        }
    }
}

void setPhase(phaseType phase){
    if(phaseStatus != phase){
        switch (phase) {
            case P_FORWARD:
                phaseVal = HIGH;
                break;
            case P_REVERSE:
                phaseVal = LOW;
                break;
            default:
                break;
        }
        phaseStatus = phase;
    }
}

void updateOutputs(){
    #if ! DEBUG
        digitalWrite(MOTOR_FIELD_PHASE_PIN, phaseVal);
        digitalWrite(MOTOR_FIELD_VOLT_H_PIN, fieldVal);
        digitalWrite(MOTOR_FIELD_VOLT_L_PIN, fieldVal);
        for(i=0; i<MOTORS; i++){
            motorControllers[i].updateOutputs();
        }
    #endif
}


void loop() {
    readInputs();
    safetyCheck();
    if(safetyStatus == S_SAFE){
        switch (throttleSensor.getStatus()) {
            case TH_ZERO:
                setVehicleMode(M_NEUTRAL);
              break;
            case TH_NORMAL:
                switch (modeSwitch) {
                    case LOW:
                        setVehicleMode(M_FORWARD);
                        break;
                    case HIGH:
                        setVehicleMode(M_REVERSE);
                        break;
                    default:
                        break;
                }
                break;
            case TH_BOOST:
                switch (modeSwitch) {
                    case LOW:
                        setVehicleMode(M_FORWARD_BOOST);
                        break;
                    case HIGH:
                        setVehicleMode(M_REVERSE);
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
            motorControllers[i].setThrottle(throttleSensor.getSensorVal());
        }

        updateOutputs();
    }    
    if(DEBUG){
        printDebugInfo();
    }
}

char* digitalStatusString(int digitalValue){
    switch (digitalValue) {
        case HIGH:
          return "ON ";
          break;
        case LOW:
          return "OFF";
          break;
        default:
          return "???";
    }
}

char* throttleStatusString(throttleStatusType throttleStatus){
    switch (throttleStatus) {
        case TH_ZERO:
          return "ZER";
          break;
        case TH_NORMAL:
          return "NOR";
          break;
        case TH_BOOST:
          return "BST";
          break;
        default:
          break;
    }
  return "???";
}

// char* throttleNormString(double throttleNormalized){
//     char buffer[50];
//     // int percentage = (int)(100* throttleNormalized);
//     // sprintf(buffer, "%3d", percentage);
//     // itoa(percentage, buffer, 10);
//     dtostrf(throttleNormalized,0,3,buffer);
//     // sprintf(buffer, "%s",  ) ;
//     return buffer;
// }

// char* tempStatusString(tempStatusType tempStatus){
//     switch (tempStatus) {
//         case T_HOT:
//           return "HOT";
//           break;
//         case T_COLD:
//           return "CLD";
//           break;
//         case T_NORMAL:
//           return "NRM";
//           break;
//         case T_REGULATED:
//           return "REG";
//           break;
//         default:
//           break;
//     }
//     return "???";
// }

// char* tempValString(double tempVal){
//     char buffer[50];
//     dtostrf(tempVal, 0,3, buffer);
//     return buffer;
// }

void printDebugInfo(){
    char buffer[BUFFSIZE];
    int charsPrinted = 0;
    char*start = buffer;
    int throttleRawVal = throttleSensor.getRawVal();
    double throttleSensorVal = throttleSensor.getSensorVal();
    throttleStatusType throttleStatus = throttleSensor.getStatus();
    int throttleSensorTableSize = throttleSensor.getSensorTableSize();

    charsPrinted += snprintf((start+charsPrinted), abs(BUFFSIZE-charsPrinted), "KSW:%3s|",digitalStatusString(killSwitch));
    charsPrinted += snprintf((start+charsPrinted), abs(BUFFSIZE-charsPrinted), "TST:%3d|",throttleSensorTableSize);
    charsPrinted += throttleSensor.snprintSensorTable((start+charsPrinted), abs(BUFFSIZE-charsPrinted));
    // charsPrinted += snprintf((start+charsPrinted), BUFFSIZE - charsPrinted, "THR:%3d|",(throttleRawVal));
    // charsPrinted += snprintf((start+charsPrinted), BUFFSIZE - charsPrinted, "THP:%3d|",(int)(100 * throttleSensorVal));
    // charsPrinted += snprintf((start+charsPrinted), BUFFSIZE - charsPrinted, "THS:%3s|",throttleStatusString(throttleStatus));





    // char * throttleNormStringVal = throttleNormString(throttleNormalized);
    // Serial.print(throttleNormStringVal);
    // Serial.print(",");
    // Serial.print((unsigned int)&throttleNormStringVal, HEX );
    // Serial.print("|");
    // char * tempValStringVal = tempValString(systemTempVal);
    // Serial.print(tempValStringVal);
    // Serial.print(",");
    // Serial.print((unsigned int)&tempValStringVal, HEX );
    // Serial.print("|");
    // char * tempStatusStringVal = tempStatusString(TemperatureSensor.getTempStatus());
    // Serial.print(tempStatusStringVal);
    // Serial.print(",");
    // Serial.print((unsigned int)&tempStatusStringVal, HEX );
    // Serial.print("|");
    // Serial.println();

    // char* parameters[] = {
    //     // "SSS",
    //     "KSW",
    //     // "VMS",
    //     "THS",
    //     // "THZ",
    //     // "THN",
    //     // "THB",
    //     "THV",
    //     "TMS",
    //     "TMV",
    //     "TMR"
    // };
    // char* values[] = {
    //     // SSS
    //     digitalStatusString(killSwitch),
    //     // VMS
    //     throttleStatusString(throttleSensor.getThrottleStatus()),
    //     throttleNormStringVal,
    //     tempStatusStringVal
    // };
    // int parLen = sizeof(parameters) / sizeof(parameters[0]);
    // int valLen = sizeof(values) / sizeof(values[0]);
    // int minLen = min(parLen, valLen);
    // int i;
    // for(i=0; i<minLen; i++){
    //     if(DEBUG) {
    //         Serial.print('|');
    //         Serial.print(parameters[i]);
    //         // Serial.print((int)parameters[i][3]);
    //         Serial.print(':');
    //         Serial.print(values[i]);
    //         // int j;
    //         // for(j=0; j<4; j++){
    //         //     Serial.print(values[i][j]);
    //         // }
    //         // Serial.print((int)values[i][3]);
    //         // free(parameters[i]);
    //         // free(values[i]);
    //     }
    // }
    Serial.println(buffer); delay(100);
    // |KSW|VMP|THS|THR|SSS|SMD - system
    // TPN|ASN|AVN|FVN|FPN| - per motor

}


