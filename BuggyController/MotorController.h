#ifndef MotorController_h
#define MotorController_h

#include "Arduino.h"
#include "Temperature.h"
#include "Current.h"
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

typedef enum {M_NEUTRAL, M_REVERSE, M_FORWARD, M_FORWARD_BOOST}     motorModeType;
typedef enum {T_COLD, T_NORMAL, T_REGULATED, T_HOT}                tempStatusType;
typedef enum {A_NORMAL, A_REGULATED, A_HIGH}                       armStatusType;
typedef enum {P_FORWARD, P_REVERSE}                                 phaseType;

class MotorController{
public:
    MotorController();
    // MotorController(int tempPin, int armSensePin, int armVoltPin, int fieldVoltPin, int fieldPhasePin);
    void            setPins(int tempPin, int armSensePin, int armVoltPin, int fieldVoltPin, int fieldPhasePin);
    void            setTempBounds(int tempType, double minTemp, double regTemp, double maxTemp);
    void            setArmBounds(int armType, double regArm, double maxArm);
    void            setMotorMode(motorModeType motorMode);
    void            setThrottle(double throttleVal);
    void            initPins();
    void            readInputs();
    tempStatusType  getTempStatus();
    armStatusType   getArmStatus();
    // void            setPIDs(double kp, double ki, double kd);
    void            updateOutputs();
    void            shutdown();

private:
    int             _tempPin;
    int             _armSensePin;
    int             _armVoltPin;
    int             _fieldVoltPin;
    int             _fieldPhasePin;
    motorModeType   _motorMode;
    phaseType       _phaseStatus;
    double          _throttleVal;

    double          _tempVal;
    int             _tempType;
    double          _minTemp;
    double          _regTemp;    
    double          _maxTemp;
    
    double          _armVal;
    int             _armType;
    double          _regArm;
    double          _maxArm;
    
    int             _phaseVal;
    int             _armVoltVal;
    int             _fieldVoltVal;
    void            setPhase(phaseType phase);
};

#endif