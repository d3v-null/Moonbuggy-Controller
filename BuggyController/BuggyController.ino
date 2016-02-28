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

// Safety Stuff
safetyStatusType safetyStatus; // Safety status of system, starts off as safe

// throttleStatusType throttleStatus;
motorModeType vehicleMode; // Mode of all motors, starts off as Neutral

NormalizedVoltageSensor throttleSensor;

TemperatureSensor systemTempSensor;





// int throttleRaw;
// double throttleNormalized; // The normalized value of the throttle (0.0 -> 1.0)

int killSwitch;  // The value of the killSwitch sensor

int modeSwitch; // The value of the modeSwitch sensor

double systemTempVal;

MotorController motorControllers[MOTORS];

throttleStatusNode constructThrottleStatusNode(double threshold, throttleStatusType throttleStatus){
    throttleStatusNode node;
    node.threshold = threshold;
    node.statusVal = throttleStatus;
    return node;
}

throttleStatusType getThrottleStatus(){
    throttleNormalized = throttleSensor.getSensorVal();
    throttleStatusType throttleStatus = TH_BOOST;
    throttleStatusNode statuses[] = {
        constructThrottleStatusNode(THROTTLE_THRESHOLD_ZERO,  TH_ZERO),
        constructThrottleStatusNode(THROTTLE_THRESHOLD_BOOST, TH_NORMAL)
    };
    int statusLen = sizeof(statuses) / sizeof(statuses[0]);
    int i;
    for( i=0; i<statusLen; i++) {
        if(throttleNormalized < statuses[i].threshold){
            throttleStatus = statuses[i].statusVal;
            break;
        } else {
        }
    }
    char buffer[50];
    // Serial.println( sprintf(buffer, "returning %d", (int)(throttleStatus) ) );
    return throttleStatus;
}

tempStatusType getTempStatus(){
    return TemperatureSensor::getTempStatus(systemTempVal, ONBOARD_MINTEMP, ONBOARD_REGTEMP, ONBOARD_MAXTEMP, IGNORE_TEMPS);
}

batteryStatusNode constructBatteryStatusNode(double threshold, batterStatusType batteryStatus){
    batteryStatusNode node;
    node.threshold = threshold;
    node.statusVal = batteryStatus;
    return node;
}

batteryStatusType getBatteryStatus()

void readInputs() {
    // Store the normalized value of the throttle
    // throttleRaw = analogRead(THROTTLE_PIN);
    // throttleNormalized = normalize(throttleRaw, THROTTLE_MIN, THROTTLE_MAX);
    throttleSensor.readInputs();


    if(!IGNORE_TEMPS) systemTempVal = TemperatureSensor::analog2temp(analogRead(ONBOARD_TEMP_PIN), ONBOARD_TEMP_PIN);

    //  map((double)throttleRaw, (double)THROTTLE_MIN, (double)THROTTLE_MAX, 0.0, 1.0);
    // throttleNormalized = min(max(throttleNormalized, 0.0), 1.0);
    // Process the status of the throttle
    // throttleStatus = 
    // Store the value of killSwitch in killSwitch;
    killSwitch = digitalRead(KILLSWITCH_PIN);

    if(!IGNORE_MODE) modeSwitch = digitalRead(VEHICLE_MODE_PIN);

    int i;
    for(i=0; i<MOTORS; i++){
        motorControllers[i].readInputs();
    }
}

void shutdown() {
    if(DEBUG) Serial.println("Shutting Down");

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
                if(! IGNORE_CURRENTS and motorControllers[i].getArmStatus() == A_HIGH){
                    if(DEBUG) Serial.print("Failed current check on MOTOR: A_HIGH, ");
                    if(DEBUG) Serial.println(i);
                    shouldTerminate = true;
                    break;
                }
            }
            if(!shouldTerminate and killSwitch == HIGH){
                if(DEBUG) Serial.println("Killswitch Engaged");
                shouldTerminate = true;
            }
            if(!shouldTerminate and !IGNORE_TEMPS){
                switch(getTempStatus()){
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
    if(DEBUG) Serial.begin(BAUDRATE);
    if(DEBUG) Serial.println("Initializing");
    delay(1000);

    //Set pinmodes
    // pinMode(THROTTLE_PIN, INPUT);
    throttleSensor = NormalizedVoltageSensor();
    throttleSensor.setPins(THROTTLE_PIN);
    throttleSensor.initPins();

    systemTempSensor = TemperatureSensor();
    systemTempSensor.setPins(ONBOARD_TEMP_PIN);
    systemTempSensor.initPins();
    

    pinMode(KILLSWITCH_PIN, INPUT);
    if(! IGNORE_MODE) pinMode(VEHICLE_MODE_PIN, INPUT);

    if(MOTORS > 0){
        Serial.println("Initializing Motor 1");
        motorControllers[0].setPins(
            MOTOR_0_TEMP_PIN,
            MOTOR_0_ARM_SENSE_PIN,
            MOTOR_0_ARM_VOLT_PIN,
            // MOTOR_0_FIELD_VOLT_PIN,
            // MOTOR_0_FIELD_PHASE_PIN
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
        if(DEBUG) Serial.println("Initializing Motor 2");
        motorControllers[1].setPins(
            MOTOR_1_TEMP_PIN,
            MOTOR_1_ARM_SENSE_PIN,
            MOTOR_1_ARM_VOLT_PIN,
            // MOTOR_1_FIELD_VOLT_PIN,
            // MOTOR_1_FIELD_PHASE_PIN
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

    pinMode( MOTOR_FIELD_VOLT_PIN, OUTPUT );
    pinMode( MOTOR_FIELD_PHASE_PIN, OUTPUT);

    throttleNormalized = 0.0;
    // throttleStatus = TH_ZERO;
    vehicleMode = M_NEUTRAL;
    S_SAFE
    modeSwitch = LOW;
    systemTempVal = 0.0;
    phaseStatus = P_FORWARD;
    phaseVal = HIGH;
}

void setVehicleMode(motorModeType motorMode = M_NEUTRAL){
    for(i=0; i<MOTORS; i++){
        motorControllers[i].setMotorMode(vehicleMode);
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
    digitalWrite(MOTOR_FIELD_PHASE_PIN, phaseVal);
    digitalWrite(MOTOR_FIELD_VOLT_H_PIN, fieldVal);
    digitalWrite(MOTOR_FIELD_VOLT_L_PIN, fieldVal);
    for(i=0; i<MOTORS; i++){
        motorControllers[i].updateOutputs();
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

char* throttleNormString(double throttleNormalized){
    char buffer[50];
    // int percentage = (int)(100* throttleNormalized);
    // sprintf(buffer, "%3d", percentage);
    // itoa(percentage, buffer, 10);
    dtostrf(throttleNormalized,0,3,buffer);
    // sprintf(buffer, "%s",  ) ;
    return buffer;
}

char* tempStatusString(tempStatusType tempStatus){
    switch (tempStatus) {
        case T_HOT:
          return "HOT";
          break;
        case T_COLD:
          return "CLD";
          break;
        case T_NORMAL:
          return "NRM";
          break;
        case T_REGULATED:
          return "REG";
          break;
        default:
          break;
    }
    return "???";
}

char* tempValString(double tempVal){
    char buffer[50];
    dtostrf(tempVal, 0,3, buffer);
    return buffer;
}

void printDebugInfo(){
    char * throttleNormStringVal = throttleNormString(throttleNormalized);
    Serial.print(throttleNormStringVal);
    Serial.print(",");
    Serial.print((unsigned int)&throttleNormStringVal, HEX );
    Serial.print("|");
    char * tempValStringVal = tempValString(systemTempVal);
    Serial.print(tempValStringVal);
    Serial.print(",");
    Serial.print((unsigned int)&tempValStringVal, HEX );
    Serial.print("|");
    char * tempStatusStringVal = tempStatusString(getTempStatus());
    Serial.print(tempStatusStringVal);
    Serial.print(",");
    Serial.print((unsigned int)&tempStatusStringVal, HEX );
    Serial.print("|");
    Serial.println();

    char* parameters[] = {
        // "SSS",
        "KSW",
        // "VMS",
        "THS",
        // "THZ",
        // "THN",
        // "THB",
        "THV",
        "TMS",
        "TMV",
        "TMR"
    };
    char* values[] = {
        // SSS
        digitalStatusString(killSwitch),
        // VMS
        throttleStatusString(getThrottleStatus(throttleNormalized)),
        // digitalStatusString((int)(throttleNormalized < THROTTLE_THRESHOLD_ZERO) ),
        // digitalStatusString((int)(throttleNormalized >= THROTTLE_THRESHOLD_ZERO and throttleNormalized < THROTTLE_THRESHOLD_BOOST) ),
        // digitalStatusString((int)(throttleNormalized >= THROTTLE_THRESHOLD_BOOST)),
        // throttleStatusString(getThrottleStatus((double)0.5)),
        throttleNormStringVal,
        tempStatusStringVal,
        

    };
    int parLen = sizeof(parameters) / sizeof(parameters[0]);
    int valLen = sizeof(values) / sizeof(values[0]);
    int minLen = min(parLen, valLen);
    int i;
    for(i=0; i<minLen; i++){
        if(DEBUG) {
            Serial.print('|');
            Serial.print(parameters[i]);
            // Serial.print((int)parameters[i][3]);
            Serial.print(':');
            Serial.print(values[i]);
            // int j;
            // for(j=0; j<4; j++){
            //     Serial.print(values[i][j]);
            // }
            // Serial.print((int)values[i][3]);
            // free(parameters[i]);
            // free(values[i]);
        }
    }
    Serial.println();
    // |KSW|VMP|THS|THR|SSS|SMD - system
    // TPN|ASN|AVN|FVN|FPN| - per motor

}

void loop() {
    readInputs();
    safetyCheck();
    if(safetyStatus == S_SAFE){
        // set vehicle mode
        switch (getThrottleStatus(throttleNormalized)) {
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

