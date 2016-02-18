#ifndef Morse_h
#define Morse_h

#include "Arduino.h"
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

typedef enum {M_NEUTRAL, M_REVERSE, M_FORWARD, M_FORWARD_BOOST} motorModeType;

class MotorController{
public:
    MotorController();
    MotorController(int tempPin, int armSensePin, int armVoltPin, int fieldVoltPin, int fieldPhasePin);
    void setPins(int tempPin, int armSensePin, int armVoltPin, int fieldVoltPin, int fieldPhasePin);
    void setMotorMode(motorModeType motorMode);
    void setThrottle(double throttleVal);
    void initPins();
    void updatePIDs(double kp, double ki, double kd);
    void updateOutputs();
    void shutDown();

private:
    int _tempPin;
    int _armSensePin;
    int _armVoltPin;
    int _fieldVoltPin;
    int _fieldPhasePin;
    motorModeType _motorMode;
    double _throttleVal;
};

#endif