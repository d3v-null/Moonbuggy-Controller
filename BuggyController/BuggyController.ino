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
    #ifndef IGNORE_SYS_TEMP
        TemperatureSensor systemTempSensor;
    #endif
#endif
#ifndef IGNORE_CURRENTS
    #ifdef DEBUG_CURRENTS
        CurrentSensor debugCurrentSensor;
    #endif
#endif

// The value of the killSwitch sensor
int killSwitch; 

// The value of the modeSwitch sensor
int modeSwitch; 

// the value of the phase pin
int phaseVal;

// the value of the field 
// int fieldVal;

// An array pointing to the motor controllers
MotorController motorControllers[MOTORS];

unsigned long lastDebug;
unsigned long lastRead;

void setup() {

    if(DEBUG) {Serial.begin(BAUDRATE); delay(DEBUG_PRINT_TIME);} 
    if(DEBUG) {Serial.println("Initializing"); delay(DEBUG_PRINT_TIME);} 

    if(DEBUG) {Serial.println("Initializing Killswitch"); delay(DEBUG_PRINT_TIME);} 
    pinMode(KILLSWITCH_PIN, INPUT);

    char buffer[DEBUG_BUFSIZ];//
    char*start; 
    int charsPrinted;

    #ifndef IGNORE_THROTTLE
        //Set pinmodes
        // pinMode(THROTTLE_PIN, INPUT);
        if(DEBUG){ Serial.println("Constructing Throttle object"); delay(DEBUG_PRINT_TIME);}
        throttleSensor = ThrottleSensor();
        throttleSensor.init();
        throttleSensor.setPins(THROTTLE_PIN);
        throttleSensor.initPins();
        throttleSensor.setInputConstraints(THROTTLE_RAW_MIN, THROTTLE_RAW_MAX);
        throttleSensor.setStatusBounds(THROTTLE_THRESHOLD_ZERO, THROTTLE_THRESHOLD_BOOST);
    #endif


    #ifndef IGNORE_TEMPS
        #ifndef IGNORE_SYS_TEMP
            if(DEBUG) {Serial.println("Constructing TemperatureSensor"); delay(DEBUG_PRINT_TIME);} 

            systemTempSensor = TemperatureSensor();
            systemTempSensor.init();
            systemTempSensor.setPins(ONBOARD_TEMP_PIN);
            systemTempSensor.initPins();
            systemTempSensor.setStatusBounds(ONBOARD_MINTEMP, ONBOARD_REGTEMP, ONBOARD_MAXTEMP);
        #endif
    #endif

    #ifndef IGNORE_BATTERY

        if(DEBUG) {Serial.println("Constructing Battery"); delay(DEBUG_PRINT_TIME);} 

        batterySensor = BatterySensor();
        batterySensor.init();
        batterySensor.setPins(BATTERY_VOLT_PIN);
        batterySensor.initPins();
        batterySensor.setSensorMultiplier(BATTERY_SENSOR_MULTIPLIER);
        batterySensor.setStatusBounds(BATTERY_MIN, BATTERY_MAX);
    
    #endif

    #ifndef IGNORE_CURRENTS
        #ifdef DEBUG_CURRENTS
            debugCurrentSensor = CurrentSensor();
            debugCurrentSensor.init();
            debugCurrentSensor.setPins(DEBUG_CURRENT_PIN);
            debugCurrentSensor.initPins();
            debugCurrentSensor.setStatusBounds(MOTOR_0_REG_CURRENT, MOTOR_0_MAX_CURRENT);
        #endif
    #endif

    #ifndef IGNORE_MODE
        if(DEBUG) {Serial.println("Initializing VehicleMode"); delay(DEBUG_PRINT_TIME);} 
        pinMode(VEHICLE_MODE_PIN, INPUT);
    #endif

    #if MOTORS > 0
        Serial.println("Initializing Fields");
        pinMode( MOTOR_FIELD_VOLT_PIN, OUTPUT );
        pinMode( MOTOR_FIELD_PHASE_PIN, OUTPUT);

        Serial.println("Initializing Motor 0");
        motorControllers[0].setPins(
            MOTOR_0_TEMP_PIN,
            MOTOR_0_ARM_SENSE_PIN,
            MOTOR_0_ARM_VOLT_PIN
        );
        motorControllers[0].setTempBounds(
            MOTOR_0_MINTEMP,
            MOTOR_0_REGTEMP,
            MOTOR_0_MAXTEMP
        );
        motorControllers[0].setArmBounds(
            MOTOR_0_REG_CURRENT,
            MOTOR_0_MAX_CURRENT
        );
        motorControllers[0].initPins();
    #endif

    #if MOTORS > 1
        if(DEBUG) Serial.println("Initializing Motor 1");
        motorControllers[1].setPins(
            MOTOR_1_TEMP_PIN,
            MOTOR_1_ARM_SENSE_PIN,
            MOTOR_1_ARM_VOLT_PIN
        );
        motorControllers[1].setTempBounds(
            MOTOR_1_MINTEMP,
            MOTOR_1_REGTEMP,
            MOTOR_1_MAXTEMP
        );
        motorControllers[1].setArmBounds(
            MOTOR_1_REG_CURRENT,
            MOTOR_1_MAX_CURRENT
        );
        motorControllers[1].initPins();
    #endif

    vehicleMode = M_NEUTRAL;
    safetyStatus = S_SAFE;
    modeSwitch = LOW;
    phaseStatus = P_FORWARD;
    phaseVal = HIGH;
    killSwitch = LOW;
    if(DEBUG) Serial.println("Finished Initializing"); delay(DEBUG_PRINT_TIME);

    #ifdef DATA_LOGGING
        char header[DEBUG_BUFSIZ];
        int charsUsed = 0;
        int charsRemaining = 0;
        charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"TIME\"" );
        charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
        #ifndef IGNORE_THROTTLE
            charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"THRV\"" );
            charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
            #ifdef CALLIBRATE_SENSORS
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"THRR\"" );
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
            #endif
        #endif

        #ifndef IGNORE_TEMPS
            #ifndef IGNORE_SYS_TEMP
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"TMPV\"" );
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
                #ifdef CALLIBRATE_SENSORS
                    charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"TMPR\"" );
                    charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
                #endif
            #endif
        #endif

        #ifndef IGNORE_BATTERY
            charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"BATV\"" );
            charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
            #ifdef CALLIBRATE_SENSORS
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"BATR\"" );
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
            #endif
        #endif

        #ifndef IGNORE_CURRENTS
            #ifdef DEBUG_CURRENTS
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"CURV\"");
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
                #ifdef CALLIBRATE_SENSORS
                    charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"CURR\"");
                    charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
                #endif
            #endif
        #endif

        #if MOTORS > 0
            charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"FLDV\"");
            charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);

            for(int i=0; i<MOTORS; i++){
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"M%dTH\"", i);
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"M%dAR\"", i);
                charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
                #ifndef IGNORE_TEMPS
                    charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"M%dTV\"", i );
                    charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
                    #ifdef CALLIBRATE_SENSORS
                        charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"M%dTR\"", i );
                        charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
                    #endif
                #endif
                #ifndef IGNORE_CURRENTS
                    charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"M%dCV\"", i);
                    charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
                    #ifdef CALLIBRATE_SENSORS
                        charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), "\"M%dCR\"", i);
                        charsUsed += snprintf((header+charsUsed), abs(DEBUG_BUFSIZ-charsUsed), DEBUG_DELIMETER);
                    #endif
                #endif
            }
            
        #endif

        Serial.println(header);
    #endif

    unsigned long now = millis();
    readInputs();
    lastRead = now;

}

void readInputs() {
    // Store the value of the kill switch in killSwitch;
    #ifndef IGNORE_KILLSWITCH
        killSwitch = digitalRead(KILLSWITCH_PIN);
    #endif
    #ifndef IGNORE_THROTTLE
        throttleSensor.readInputs();
    #endif
    #ifndef IGNORE_TEMPS
        #ifndef IGNORE_SYS_TEMP
            systemTempSensor.readInputs();
        #endif
    #endif
    #ifndef IGNORE_BATTERY
        batterySensor.readInputs();
    #endif
    #ifndef IGNORE_CURRENTS
        #ifdef DEBUG_CURRENTS
            debugCurrentSensor.readInputs();
        #endif
    #endif
    #ifndef IGNORE_MODE
        modeSwitch = digitalRead(VEHICLE_MODE_PIN);
    #endif

    for(int i=0; i<MOTORS; i++){
        motorControllers[i].readInputs();
    }
}


/** temporarily shut down engines while not in use */
void shutdown() {
    if(DEBUG) Serial.println("Shutting Down");

    vehicleMode = M_NEUTRAL;
    // fieldVal = FIELD_OFF;
    for(int i=0; i<MOTORS; i++){
        motorControllers[i].shutdown();
    }   
}

/* Terminate system completely until controller is reset */
void terminate() {
    safetyStatus = S_TERMINATING;
    shutdown();
    safetyStatus = S_TERMINATED;
}

/**
 * If system thinks that it is safe, performs a check of safety parameters, 
 * updates variable safetyStatus
 */
void safetyCheck() {
    // if(DEBUG) Serial.println("Starting safety check"); delay(DEBUG_PRINT_TIME);
    boolean shouldTerminate = false;
    char dispbuffer[DISP_BUFSIZ] = "";
    switch (safetyStatus) {
        case S_SAFE:
        case S_INITIAL:
            #ifndef IGNORE_KILLSWITCH
                if(!shouldTerminate and killSwitch == HIGH){
                    snprintf(dispbuffer, DISP_BUFSIZ, "FAIL KSW: ON");
                    shouldTerminate = true;
                }
            #endif
            #ifndef IGNORE_TEMPS
                if(!shouldTerminate){
                    for(int i=0; i<MOTORS; i++){
                        switch (motorControllers[i].getTempStatus()) {
                            case T_COLD:
                              snprintf(dispbuffer, DISP_BUFSIZ, "FAIL TMP MOTOR: T_COLD, %d", i);
                              shouldTerminate = true;
                              break;
                            case T_HOT:
                              snprintf(dispbuffer, DISP_BUFSIZ, "FAIL TMP MOTOR: T_HOT, %d", i);
                              shouldTerminate = true;
                              break;
                            default:
                              break;
                        }
                        if( shouldTerminate ){ break; }
                    }
                }
                #ifndef IGNORE_SYS_TEMP
                    if(!shouldTerminate){                    
                        switch(systemTempSensor.getStatus()){
                            case T_COLD:
                              snprintf(dispbuffer, DISP_BUFSIZ, "FAIL TMP: T_COLD");
                              shouldTerminate = true;
                              break;
                            case T_HOT:
                              snprintf(dispbuffer, DISP_BUFSIZ, "FAIL TMP: T_HOT");
                              shouldTerminate = true;
                              break;
                            default:
                              break;
                        }
                    }
                #endif
            #endif

            #ifndef IGNORE_CURRENTS
                #ifdef DEBUG_CURRENTS
                    if(!shouldTerminate){
                        if(debugCurrentSensor.getStatus() == C_HIGH){
                            snprintf(dispbuffer, DISP_BUFSIZ, "FAIL CUR: C_HIGH, ");
                            shouldTerminate = true;
                            break;
                        }
                    }
                #endif
                #if MOTORS > 0
                    if(!shouldTerminate){
                        for(int i=0; i<MOTORS; i++){
                            if(motorControllers[i].getArmStatus() == C_HIGH){
                                snprintf(dispbuffer, DISP_BUFSIZ, "FAIL CUR MOTOR: C_HIGH, %d", i);
                                shouldTerminate = true;
                                break;
                            }
                        }
                    }
                #endif
            #endif

            #ifndef IGNORE_BATTERY
                if(!shouldTerminate){
                    switch(batterySensor.getStatus()){
                        case B_LOW:
                            snprintf(dispbuffer, DISP_BUFSIZ, "FAIL BAT: B_LOW");
                            shouldTerminate = true;
                            break;
                        case B_HIGH:
                            snprintf(dispbuffer, DISP_BUFSIZ, "FAIL BAT: B_HIGH");       
                            shouldTerminate = true;
                            break;
                        default:
                            break;
                    }
                }
            #endif

            // terminates system if necessary
            if(shouldTerminate){
                if(DEBUG) Serial.println(dispbuffer);
                terminate();
            }
            break;
        case S_TERMINATING:
            // Should not be terminating
            if(DEBUG) Serial.println("terminating?");
            terminate();
            safetyStatus = S_TERMINATED;
            break;
        default:
            break;
    }
    // if(DEBUG) Serial.println("Finished safety check"); delay(DEBUG_PRINT_TIME);
}

void setVehicleMode(motorModeType motorMode = M_NEUTRAL){
    // if(DEBUG) Serial.println("Setting Vehicle Mode"); delay(DEBUG_PRINT_TIME);
    if(vehicleMode != motorMode){
        switch(motorMode){
            case M_REVERSE:
                setPhase(P_REVERSE);
                break;
            case M_FORWARD:
            case M_FORWARD_BOOST:
                setPhase(P_FORWARD);
                break;
            case M_NEUTRAL:
            default:
                break;
        }
        vehicleMode = motorMode;
        for(int i=0; i<MOTORS; i++){
            motorControllers[i].setMotorMode(vehicleMode);
        }
    }
}

int getFieldVal(){
    int fieldVal = FIELD_OFF;
    if(safetyStatus == S_SAFE){
        double currentVoltage = FIELD_TARGET_VOLT;
        #ifndef IGNORE_BATTERY
            currentVoltage = batterySensor.getSensorVal();
        #endif
        switch(vehicleMode){
            case M_FORWARD_BOOST:
                //boost disabled
                // fieldVal = (int)(FIELD_BOOST * min(1.0, targetVoltage / currentVoltage) );
                // break;
            case M_REVERSE:
            case M_FORWARD:
                fieldVal = (int)(FIELD_ON * min(1.0, FIELD_TARGET_VOLT / currentVoltage) );
                break;
            case M_NEUTRAL:
            default:
                break;
        }
    } 
    return fieldVal;
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
    digitalWrite(MOTOR_FIELD_VOLT_PIN, getFieldVal() );
    for(int i=0; i<MOTORS; i++){
        motorControllers[i].updateOutputs();
    }
}


void loop() {
    unsigned long now = millis();
    if(now - lastRead > SENSOR_READ_TIME){
        readInputs();
        lastRead = now;
    }
    
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
        for(int i=0; i<MOTORS; i++){
            motorControllers[i].setThrottle(throttleSensor.getSensorVal());
        }

        updateOutputs();
    }    
    if(DEBUG){
        if( now - lastDebug > DEBUG_PRINT_TIME ){
            lastDebug = now;
            char buffer[DEBUG_BUFSIZ] = "";
            snprintDebugInfo(buffer, DEBUG_BUFSIZ);
            Serial.println(buffer); 
        }
    }
}

void snprintDebugInfo(char* buffer, int charsRemaining){
    int charsPrinted = 0;

    #ifdef DATA_LOGGING
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "%6d", millis() % 1000000 );
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    #else
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "SSS:");
        charsPrinted += snprintSafetyStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), safetyStatus);
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);

        // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "VMD:");
        // charsPrinted += snprintVehicleModeString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), vehicleMode);
        // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);

        // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "PHS:");
        // charsPrinted += snprintPhaseString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), phaseStatus);
        // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);

        // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "KSW:%3s",digitalStatusString(killSwitch));
        // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    #endif

    #ifndef IGNORE_THROTTLE
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "THR<"); 
        #endif
        charsPrinted += throttleSensor.snprintReadings((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), ">");
        #endif
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    #endif

    #ifndef IGNORE_TEMPS
        #ifndef IGNORE_SYS_TEMP
            #ifndef DATA_LOGGING
                charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "TMP<"); 
            #endif
            charsPrinted += systemTempSensor.snprintReadings((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
            #ifndef DATA_LOGGING
                charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), ">");
            #endif
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
        #endif
    #endif

    #ifndef IGNORE_BATTERY
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "BAT<");
        #endif
        charsPrinted += batterySensor.snprintReadings((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), ">");
        #endif
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    #endif

    #ifndef IGNORE_CURRENTS
        #ifdef DEBUG_CURRENTS
            #ifndef DATA_LOGGING
                charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "CUR<"); 2
            #endif
            charsPrinted += debugCurrentSensor.snprintReadings((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
            #ifndef DATA_LOGGING
                charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), ">");
            #endif
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
        #endif
    #endif

    #if MOTORS > 0
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "FLD<");
        #endif
        int fieldVal = getFieldVal() ;
        #ifdef ENABLE_NORMALIZATION
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_CELL_FMT_D, map(fieldVal, 0, SYSTEM_PWM_MAX, 0, 100) );
        #else
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_CELL_FMT_D, fieldVal);
        #endif
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), ">");
        #endif
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);

        for(int i=0; i<MOTORS; i++){
            #ifndef DATA_LOGGING
                charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "MOT%d<", i);
            #endif
            charsPrinted += motorControllers[i].snprintParameters((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
            #ifndef DATA_LOGGING
                charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), ">", i);
            #endif
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
        }
    #endif
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

int snprintSafetyStatusString(char* buffer, int charsRemaining, safetyStatusType statusVal ){
    char* statusStr = "???";
    switch ( statusVal ) {
        case S_INITIAL:
          statusStr = "INI";
          break;
        case S_SAFE:
          statusStr = "SFE";
          break;
        case S_TERMINATING:
          statusStr = "TMG";
          break;
        case S_TERMINATED:
          statusStr = "TMD";
          break;
        default:
          break;
    }
    return snprintf(buffer, charsRemaining, statusStr);
}
int snprintVehicleModeString(char* buffer, int charsRemaining, motorModeType modeVal ){
    char* statusStr = "???";
    switch ( modeVal ) {
        case M_NEUTRAL:
          statusStr = "NEU";
          break;
        case M_REVERSE:
          statusStr = "REV";
          break;
        case M_FORWARD:
          statusStr = "FWD";
          break;
        case M_FORWARD_BOOST:
          statusStr = "FWB";
          break;
        default:
          break;
    }
    return snprintf(buffer, charsRemaining, statusStr);
}
int snprintPhaseString(char* buffer, int charsRemaining, phaseType phase ){
    char* statusStr = "???";
    switch ( phase ) {
        case P_REVERSE:
          statusStr = "REV";
          break;
        case P_FORWARD:
          statusStr = "FWD";
          break;
        default:
          break;
    }
    return snprintf(buffer, charsRemaining, statusStr);
}


