#ifndef TemperatureSensor_h
#define TemperatureSensor_h 

#include "VoltageSensor.h"

typedef struct sensorNode {
    int input;
    double temperature;
} sensorNode;

sensorNode constructTempNode(int input, double temperature);

typedef enum {T_COLD, T_NORMAL, T_REGULATED, T_HOT} tempStatusType;

typedef struct tempStatusNode {
    double temperature;
    tempStatusType tempStatus;
} tempStatusNode;

tempStatusNode constructTempStatusNode(double temperature, tempStatusType tempStatus);

class TemperatureSensor: public VoltageSensor {
public:
    TemperatureSensor();
    // void            setPins(int sensorPin);
    // void            initPins();
    void            setSensorType(int sensorType);
    void            setTempBounds(double minTemp, double regTemp, double maxTemp, bool ignoreTemps);
    double          getTemp();
    tempStatusType  getTempStatus();
private:
    int             _sensorType;
    // int             _sensorPin;
    sensorNode*       _tempTable;
    int             _tempTableLen;
    double          _minTemp;
    double          _regTemp;
    double          _maxTemp;
    bool            _ignoreTemps;
    // int             _sensorVal;
    tempStatusNode* _tempStatusTable;
    int             _tempStatusTableLen;
};


#endif