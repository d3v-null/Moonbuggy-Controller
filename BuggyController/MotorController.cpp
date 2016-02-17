#include "Arduino.h"
#include "MotorController.h"

MotorController::MotorController(int armSensePin, int armVoltPin, int fieldVoltPin, int fieldPhasePin) {
    _armSensePin = armSensePin;
    _armVoltPin = armVoltPin;
    _fieldVoltPin = fieldVoltPin;
    _fieldPhasePin = fieldPhasePin;
    _throttleVal = 0.0;
}

void MotorController::updateThrottleVal(double throttleVal){
    _throttleVal = throttleVal;
}

void MotorController::shutDown(){
    _throttleVal = 0.0;
}