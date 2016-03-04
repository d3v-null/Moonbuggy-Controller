#ifndef MotorController_h
#define MotorController_h

#include "Arduino.h"
#include "TemperatureSensor.h"
#include "CurrentSensor.h"
#include "Normalization.h"
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

typedef enum {M_NEUTRAL, M_REVERSE, M_FORWARD, M_FORWARD_BOOST}     motorModeType;

class MotorController{
public:
    MotorController();
    void            setPins(int tempPin, int armSensePin, int armVoltPin, int fieldVoltPin, int fieldPhasePin);
    void            setTempBounds(int tempType, double minTemp, double regTemp, double maxTemp, bool ignoreTemps);
    void            setArmBounds(int armType, double regArm, double maxArm, bool ignoreCurrents);
    void            setMotorMode(motorModeType motorMode);
    void            setThrottle(double throttleVal = 0.0);
    void            initPins();
    void            readInputs();
    tempStatusType  getTempStatus();
    currentStatusType getArmStatus();
    // void            setPIDs(double kp, double ki, double kd);
    void            updateOutputs();
    void            shutdown();

private:
    TemperatureSensor _temperatureSensor;
    CurrentSensor     _armCurrentSensor;
    int             _armVoltPin;
    motorModeType   _motorMode;
    double          _throttleVal;
};

#endif