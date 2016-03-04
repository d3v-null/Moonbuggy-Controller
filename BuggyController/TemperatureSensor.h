#ifndef TemperatureSensor_h
#define TemperatureSensor_h 

#include "VoltageSensor.h"

typedef enum {T_COLD, T_NORMAL, T_REGULATED, T_HOT} tempStatusType;

typedef struct tempStatusNode {
    double          threshold;
    tempStatusType  statusVal;
} tempStatusNode;

class TemperatureSensor: public VoltageSensor {
public:
    TemperatureSensor();
    void            setSensorType(int sensorType);
    void            setStatusBounds( double minTemp=0.0, double regTemp=0.0, double maxTemp=0.0 );
    tempStatusType  getStatus();
private:
    int             _sensorType;
    // double          _minTemp;
    // double          _regTemp;
    // double          _maxTemp;
    tempStatusNode* _tempStatusTable;
    // int             _tempStatusTableLen;
};

#endif