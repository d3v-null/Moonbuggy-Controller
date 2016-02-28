#ifndef VoltageSensor_h
#define VoltageSensor_h 

#define SYSTEM_VCC 5.0
#define SYSTEM_ANALOGUE_MAX 1024

typedef struct sensorNode {
    int input;
    double sensorVal;
} sensorNode;

sensorNode constructSensorNode(int input, double sensorVal)

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
    int             _sensorTableLen;
};

class VoltageDividerSensor: public VoltageDividerSensor {
public:
    VoltageDividerSensor();
    void            setSensorMultiplier(double sensorMultiplier);
};

#endif