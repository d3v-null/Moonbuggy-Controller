#ifndef Morse_h
#define Morse_h

#include "Arduino.h"


class MotorController{
public:
    MotorController(int armSensePin, int armVoltPin, int fieldVoltPin, int fieldPhasePin);
    void updateThrottle(double throttleVal);
    void shutDown();

private:
    int _armSensePin;
    int _armVoltPin;
    int _fieldVoltPin;
    int _fieldPhasePin;
    double _throttleVal;
}

#endif