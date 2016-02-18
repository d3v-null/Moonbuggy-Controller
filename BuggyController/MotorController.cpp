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

#include "Arduino.h"
#include "MotorController.h"

MotorController::MotorController(){
    setPins();
    setMotorMode();
    setThrottle();
}

MotorController::MotorController(int tempPin, int armSensePin, int armVoltPin, int fieldVoltPin, int fieldPhasePin) {
    setPins(tempPin, armSensePin, armVoltPin, fieldVoltPin, fieldPhasePin);
    setMotorMode();
    setThrottle();
}

void MotorController::setPins(int tempPin = -1, int armSensePin = -1, int armVoltPin = -1, int fieldVoltPin = -1, int fieldPhasePin = -1) {
    _tempPin = tempPin;
    _armSensePin = armSensePin;
    _armVoltPin = armVoltPin;
    _fieldVoltPin = fieldVoltPin;
    _fieldPhasePin = fieldPhasePin;
}

void MotorController::initPins(){
    pinMode(_tempPin, INPUT);
    pinMode(_armSensePin, INPUT);
    pinMode(_armVoltPin, OUTPUT);
    pinMode(_fieldVoltPin, OUTPUT);
    pinMode(_fieldPhasePin, OUTPUT);
}

void MotorController::setMotorMode(motorModeType motorMode = M_NEUTRAL){
    _motorMode = motorMode;
}

void MotorController::setThrottle(double throttleVal = 0.0){
    _throttleVal = throttleVal;
}

void MotorController::shutDown(){
    setMotorMode();
    setThrottle();
}