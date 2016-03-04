#ifndef CurrentSensor_h
#define CurrentSensor_h 

#include "VoltageSensor.h"

// double analog2current(int rawValue, int sensorType);

typedef enum {C_NORMAL, C_REGULATED, C_HIGH}                       currentStatusType;
typedef struct currentStatusNode {
    double              threshold;
    currentStatusType   statusVal;
} currentStatusNode;

#define CURRENT_STATUS_NODES 2

class CurrentSensor: public VoltageDividerSensor {
public:
    CurrentSensor();
    void                setSensorType(int sensorType);
    void                setStatusBounds(double regCurrent=1.0, double maxCurrent=1.0);
    currentStatusType   getStatus();
protected:
    int                 _sensorType;
    currentStatusNode  _statusTable[CURRENT_STATUS_NODES];
};

#endif