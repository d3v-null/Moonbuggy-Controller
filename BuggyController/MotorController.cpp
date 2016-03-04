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
    _temperatureSensor = TemperatureSensor();
    _armCurrentSensor = CurrentSensor();
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
void MotorController::setPins(int tempPin = -1, int armSensePin = -1, int armVoltPin = -1) {
    _temperatureSensor.setPins(tempPin);
    _armCurrentSensor.setPins(armSensePin);
    // _tempPin = tempPin;
    // _armSensePin = armSensePin;
    _armVoltPin = armVoltPin;
    // _fieldVoltPin = fieldVoltPin;
    // _fieldPhasePin = fieldPhasePin;
}

void MotorController::setTempBounds(int sensorType = 1, double minTemp = 0.0, double regTemp = 0.0, double maxTemp = 0.0){
    _temperatureSensor.setSensorType(sensorType);
    _temperatureSensor.setStatusBounds(minTemp, maxTemp, regTemp, maxTemp, ignoreTemps);
}

void MotorController::setArmBounds(int sensorType = 1, double regArm = 0.0, double maxArm = 0.0){
    _armCurrentSensor.setSensorType(sensorType);
    _armCurrentSensor.setStatusBounds(regArm, maxArm);
}

void MotorController::setMotorMode(motorModeType motorMode = M_NEUTRAL){
    _motorMode = motorMode;
}

void MotorController::setThrottle(double throttleVal){
    _throttleVal = throttleVal;
}

void MotorController::initPins(){
    pinMode(_armVoltPin, OUTPUT);
}

void MotorController::readInputs(){
    _temperatureSensor.readInputs();
    _armCurrentSensor.readInputs();
}

tempStatusType MotorController::getTempStatus(){
    return _temperatureSensor.getStatus();
}


armStatusType MotorController::getArmStatus(){
    return _armCurrentSensor.getStatus();
}

void MotorController::updateOutputs(){
    switch(_motorMode){
        case M_NEUTRAL:
            _armVoltVal = 0;
            break;
            // _fieldVoltVal = 0;
        case M_FORWARD:
        case M_REVERSE:
        case M_FORWARD_BOOST:
            int throttleValDenormalized
            _armVoltVal = map(_throttleVal, 0.0, 1.0, 0, 255);
            // _fieldVoltVal = 255;
            // nothing special for boost or reg yet.
    }
    // analogWrite(_fieldVoltPin, _fieldVoltVal);
    analogWrite(_armVoltPin, _armVoltVal);
}

void MotorController::shutdown(){
    setMotorMode();
    setThrottle();
    updateOutputs();
}