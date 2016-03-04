#ifndef CurrentSensor_h
#define CurrentSensor_h 

#include "VoltageSensor.h"

// double analog2current(int rawValue, int sensorType);

typedef enum {C_NORMAL, C_REGULATED, C_HIGH}                       currentStatusType;
typedef struct currentStatusNode {
    double              threshold;
    currentStatusType   statusVal;
} currentStatusNode;


class CurrentSensor: public VoltageDividerSensor {
public:
    CurrentSensor();
    void                setStatusBounds(double regCurrent=1.0, double maxCurrent=1.0);
    currentStatusType   getStatus();
protected:
    int                 _sensorType;
    currentStatusNode*  _currentStatusTable;
};

#endif