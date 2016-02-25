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
#include "Arduino.h"

MotorController::MotorController(){
    // setPins();
    _motorMode = M_NEUTRAL;
    _phaseVal = P_FORWARD;
    // setThrottle();
    // setTempBounds();
}

// MotorController::MotorController(int tempPin, int armSensePin, int armVoltPin, int fieldVoltPin, int fieldPhasePin) {
//     setPins(tempPin, armSensePin, armVoltPin, fieldVoltPin, fieldPhasePin);
//     setMotorMode();
//     setThrottle();
// }

void MotorController::setPins(int tempPin = -1, int armSensePin = -1, int armVoltPin = -1, int fieldVoltPin = -1, int fieldPhasePin = -1) {
    _tempPin = tempPin;
    _armSensePin = armSensePin;
    _armVoltPin = armVoltPin;
    _fieldVoltPin = fieldVoltPin;
    _fieldPhasePin = fieldPhasePin;
}

void MotorController::setTempBounds(int tempType = 1, double minTemp = 0.0, double regTemp = 0.0, double maxTemp = 0.0, bool ignoreTemps = false){
    _tempType = tempType;
    _minTemp = minTemp;
    _regTemp = regTemp;
    _maxTemp = maxTemp;
    _ignoreTemps = ignoreTemps;
}

void MotorController::setArmBounds(int armType = 1, double regArm = 0.0, double maxArm = 0.0, bool ignoreCurrents = false){
    _armType = armType;
    _regArm = regArm;
    _maxArm = maxArm;
    _ignoreCurrents = ignoreCurrents;
}

void MotorController::setMotorMode(motorModeType motorMode = M_NEUTRAL){
    if(_motorMode != motorMode){
        switch(motorMode){
            case M_REVERSE:
                setPhase(P_REVERSE);
                break;
            case M_FORWARD:
            case M_FORWARD_BOOST:
                setPhase(P_FORWARD);
                break;
            default:
                break;
        }
        _motorMode = motorMode;
    }
}

void MotorController::setThrottle(double throttleVal){
    _throttleVal = throttleVal;
}

void MotorController::setPhase(phaseType phase){
    if(_phaseStatus != phase){
        switch (phase) {
            case P_FORWARD:
                _phaseVal = HIGH;
                break;
            case P_REVERSE:
                _phaseVal = LOW;
                break;
            default:
                break;
        }
        _phaseStatus = phase;
    }
}

void MotorController::initPins(){
    pinMode(_tempPin, INPUT);
    pinMode(_armSensePin, INPUT);
    pinMode(_armVoltPin, OUTPUT);
    pinMode(_fieldVoltPin, OUTPUT);
    pinMode(_fieldPhasePin, OUTPUT);
}

void MotorController::readInputs(){
    if(! _ignoreTemps ) { _tempVal = TemperatureSensor::analog2temp( analogRead(_tempPin), _tempType ); }
    if(! _ignoreCurrents ) { _armVal = analog2current( analogRead(_armSensePin), _armType ); }
}

tempStatusType MotorController::getTempStatus(){
    return TemperatureSensor::getTempStatus(_tempVal, _minTemp, _regTemp, _maxTemp, _ignoreTemps);
}


armStatusType MotorController::getArmStatus(){
    if( _ignoreCurrents ) { return A_NORMAL; }
    armStatusType armStatus = A_HIGH;
    int statuses[][2] = {
        {_regArm, A_NORMAL},
        {_maxArm, A_REGULATED}
    };
    int statusLen = sizeof(statuses) / sizeof(statuses[0]);
    int i;
    for(i=0; i<statusLen; i++ ){
        if(_armVal < statuses[i][0]){
            armStatus = (armStatusType) statuses[i][1];
            break;
        }
    }
    return armStatus;
}

void MotorController::updateOutputs(){
    digitalWrite(_fieldPhasePin, _phaseVal);
    switch(_motorMode){
        case M_NEUTRAL:
            _armVoltVal = 0;
            _fieldVoltVal = 0;
        case M_FORWARD:
        case M_REVERSE:
        case M_FORWARD_BOOST:
            _armVoltVal = map(_throttleVal, 0.0, 1.0, 0, 255);
            _fieldVoltVal = 255;
            // nothing special for boost or reg yet.
    }
    analogWrite(_fieldVoltPin, _fieldVoltVal);
    analogWrite(_armVoltPin, _armVoltVal);
}

void MotorController::shutdown(){
    setMotorMode();
    setThrottle();
    updateOutputs();
}