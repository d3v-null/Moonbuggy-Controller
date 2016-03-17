#ifndef VoltageSensor_h
#define VoltageSensor_h 

#include "Configuration.h"
#include "DigitalSmooth.h"

#define ARRAYLEN( array ) ( (int)(sizeof( array ) / sizeof( array[0] )) )

typedef struct sensorNode {
    int     input;
    double  sensorVal;
} sensorNode;

sensorNode constructSensorNode(int input, double sensorVal);
int snprintSensorNode(char* buffer, int remainingChars, sensorNode node);

class VoltageSensor {
public:
    VoltageSensor();
    ~VoltageSensor();
    void            setPins(int sensorPin);
    void            initPins();
    virtual void    initSmoother();
    virtual void    init();
    void            readInputs();
    virtual int     getRawVal();
    virtual int     getSmoothedVal();
    double  getSensorVal();
    //debugging:
    // virtual int     getSensorTableSize();
    virtual int     snprintSensorNode(char* buffer, int remainingChars, sensorNode node);
    virtual int     snprintSensorTable(char* buffer, int remainingChars);
    int     snprintSensorVal(char* buffer, int charsRemaining);
    virtual int     snprintValLabel(char* buffer, int charsRemaining);
    virtual int     snprintReadings(char* buffer, int charsRemaining);
protected:
    virtual void    initSensorTable();
    virtual void    populateSensorTable(int len, int rawVals[], double sensorVals[] );
    int             _sensorPin;
    bool            _pinsSet;
    bool            _pinsInit;
    bool            _smootherInit;
    int             _rawVal;
    int             _smoothedVal;
    sensorNode*     _sensorTable;
    int             _sensorTableLen;
    DigitalSmooth*  _smoother;
    // DEBUG
};

class NormalizedVoltageSensor: public VoltageSensor {
public:
    // NormalizedVoltageSensor();
    // int             getRawVal();
    int             getSmoothedVal(); 
    void    setInputConstraints(int minimum = 0, int maximum = SYSTEM_ANALOGUE_MAX);
    // int             getSensorMin();
    // int             getSensorMax();
    void            initSensorTable();
protected:
    int             _sensorMin;
    int             _sensorMax;
};

class VoltageDividerSensor: public VoltageSensor {
public:
    // VoltageDividerSensor();
    void    setSensorMultiplier(double sensorMultiplier = 1.0);
    void            initSensorTable();
protected:
    double          _sensorMultiplier;
};

#endif