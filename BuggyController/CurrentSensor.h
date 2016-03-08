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

class CurrentSensor: public VoltageSensor {
public:
    CurrentSensor();
    void                setSensorType(int sensorType);
    void                setStatusBounds(double regCurrent=1.0, double maxCurrent=1.0);
    currentStatusType   getStatus();
    void            initSensorTable();
    // DEBUG
    int                     snprintStatusNode(char* buffer, int charsRemaining, currentStatusNode node);
    int                     snprintStatusTable(char* buffer, int charsRemaining);
    int                     snprintReadings(char* buffer, int charsRemaining);
    int                     snprintStatusString(char* buffer, int charsRemaining, currentStatusType statusVal);
protected:
    // int                 _sensorType;
    currentStatusNode  _statusTable[CURRENT_STATUS_NODES];
};

#endif