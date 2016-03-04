#ifndef VoltageSensor_h
#define VoltageSensor_h 

#include "Configuration.h"

#define SIZEOFTABLE( TABLE ) sizeof( TABLE ) / sizeof( TABLE[0] )

typedef struct sensorNode {
    int input;
    double sensorVal;
} sensorNode;

sensorNode constructSensorNode(int input, double sensorVal);

class VoltageSensor {
public:
    VoltageSensor();
    void            setPins(int sensorPin);
    void            initPins();
    void            readInputs();
    int             getRawVal();
    double          getSensorVal();
protected:
    int             _sensorPin;
    bool            _pinsSet;
    bool            _pinsInit;
    int             _rawVal;
    sensorNode*     _sensorTable;
    // int             _sensorTableLen;
};

class NormalizedVoltageSensor: public VoltageSensor {
public:
    NormalizedVoltageSensor();
    double          getSensorVal();
    void            setInputConstraints(int minimum = 0, int maximum = SYSTEM_ANALOGUE_MAX);
protected:
    int             _sensorMin;
    int             _sensorMax;
};

class VoltageDividerSensor: public VoltageSensor {
public:
    VoltageDividerSensor();
    void            setSensorMultiplier(double sensorMultiplier = 1.0);
};

#endif