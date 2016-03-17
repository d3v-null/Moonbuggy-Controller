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
 * Motor Controller class
 * Controls a single shunt wound motor's field and armature voltages 
 * Contstrained by a maximum armature current and maximum temperature
 * First version will only have a single direction (phase) and will 
 * not drop field voltage.
 * Future versions will allow forward and reverse directions and 
 * will boose speed by dropping field voltage
 *
 * How it works:
 *      Initial set up:
 *      Car is stationary, mode=Neutral, Field and arm voltages are off
 *      Mode changes to forward
 */

#include "MotorController.h"

MotorController::MotorController(){
    // setPins();
    _motorMode = M_NEUTRAL;
    _temperatureSensor = new TemperatureSensor();
    _armCurrentSensor = new CurrentSensor();
    _temperatureSensor->init();
    _armCurrentSensor->init();
    // _phaseVal = P_FORWARD;
    // setThrottle();
    // setTempBounds();
}

// MotorController::MotorController(int tempPin, int armSensePin, int armVoltPin, int fieldVoltPin, int fieldPhasePin) {
//     setPins(tempPin, armSensePin, armVoltPin, fieldVoltPin, fieldPhasePin);
//     setMotorMode();
//     setThrottle();
// }

// void MotorController::setPins(int tempPin = -1, int armSensePin = -1, int armVoltPin = -1, int fieldVoltPin = -1, int fieldPhasePin = -1) {
void MotorController::setPins(int tempPin, int armSensePin, int armVoltPin) {
    _temperatureSensor->setPins(tempPin);
    _armCurrentSensor->setPins(armSensePin);
    // _tempPin = tempPin;
    // _armSensePin = armSensePin;
    _armVoltPin = armVoltPin;
    // _fieldVoltPin = fieldVoltPin;
    // _fieldPhasePin = fieldPhasePin;
}

void MotorController::setTempBounds(double minTemp, double regTemp, double maxTemp){
    // _temperatureSensor->setSensorType(sensorType);
    _temperatureSensor->setStatusBounds(minTemp, regTemp, maxTemp);
}

void MotorController::setArmBounds(double regArm, double maxArm){
    // _armCurrentSensor->setSensorType(sensorType);
    _armCurrentSensor->setStatusBounds(regArm, maxArm);
}

void MotorController::setMotorMode(motorModeType motorMode = M_NEUTRAL){
    _motorMode = motorMode;
}

void MotorController::setThrottle(double throttleVal){
    _throttleVal = throttleVal;
}

void MotorController::initPins(){
    pinMode(_armVoltPin, OUTPUT);
    _temperatureSensor->initPins();
    _armCurrentSensor->initPins();
}

void MotorController::readInputs(){
    _temperatureSensor->readInputs();
    _armCurrentSensor->readInputs();
}

tempStatusType MotorController::getTempStatus(){
    return _temperatureSensor->getStatus();
}

currentStatusType MotorController::getArmStatus(){
    return _armCurrentSensor->getStatus();
}

void MotorController::updateOutputs(){
    switch(_motorMode){
        case M_NEUTRAL:
            _armVoltVal = 0;
            break;
        case M_FORWARD:
        case M_REVERSE:
        case M_FORWARD_BOOST:
            // double current = _armCurrentSensor.getSensorVal();
            _armVoltVal = (int)(255*_throttleVal*_armCurrentSensor->getArmCoefficient());
    }
    // analogWrite(_fieldVoltPin, _fieldVoltVal);
    analogWrite(_armVoltPin, _armVoltVal);
}

int MotorController::snprintParameters(char* buffer, int charsRemaining){
    int charsPrinted = 0;

    #ifndef DATA_LOGGING
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "THV:");
    #endif
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_CELL_FMT_LT1, (int)(100 * _throttleVal) );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);

    #ifndef DATA_LOGGING
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "ARV:");
    #endif
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_CELL_FMT_D, _armVoltVal);
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);

    #ifndef IGNORE_TEMPSß
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "TMP<"); 
        #endif
        charsPrinted += _temperatureSensor->snprintReadings((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), ">"); 
        #endif
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER );
    #endif

    #ifndef IGNORE_CURRENTS
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "CUR<"); 
        #endif
        charsPrinted += _armCurrentSensor->snprintReadings((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), ">");
        #endif
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    #endif


    return charsPrinted - 1;
}

void MotorController::shutdown(){
    setMotorMode();
    setThrottle();
    updateOutputs();
}