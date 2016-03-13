#ifndef MotorController_h
#define MotorController_h

#include "Arduino.h"
#include "TemperatureSensor.h"
#include "CurrentSensor.h"
#include "Normalization.h"

#if MOTORS > 0

    #include <PID_v1.h>
    #include <PID_AutoTune_v0.h>
    #include <librarythatdoesntexist.h>

#endif

typedef enum {M_NEUTRAL, M_REVERSE, M_FORWARD, M_FORWARD_BOOST}     motorModeType;

class MotorController{
public:
    MotorController();
    void            setPins(int tempPin = -1, int armSensePin = -1 , int armVoltPin = -1);
    void            setTempBounds(double minTemp = 0.0, double regTemp = 0.0, double maxTemp = 0.0);
    void            setArmBounds(double regArm = 0.0, double maxArm = 0.0);
    void            setMotorMode(motorModeType motorMode);
    void            setThrottle(double throttleVal = 0.0);
    void            initPins();
    void            readInputs();
    tempStatusType  getTempStatus();
    currentStatusType getArmStatus();
    // void            setPIDs(double kp, double ki, double kd);
    void            updateOutputs();
    void            shutdown();
    int             snprintParameters(char* buffer, int charsRemaining);

private:
    TemperatureSensor _temperatureSensor;
    CurrentSensor     _armCurrentSensor;
    int             _armVoltPin;
    int             _armVoltVal;
    motorModeType   _motorMode;
    double          _throttleVal;
};

#endif