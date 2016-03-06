#ifndef TemperatureSensor_h
#define TemperatureSensor_h 

#include "VoltageSensor.h"

typedef enum {T_COLD, T_NORMAL, T_REGULATED, T_HOT} tempStatusType;

typedef struct tempStatusNode {
    double          threshold;
    tempStatusType  statusVal;
} tempStatusNode;

#define TEMPERATURE_STATUS_NODES 3

class TemperatureSensor: public VoltageSensor {
public:
    TemperatureSensor();
    void            setSensorType(int sensorType);
    void            setStatusBounds( double minTemp=0.0, double regTemp=0.0, double maxTemp=0.0 );
    tempStatusType  getStatus();
    // debug
    int                     snprintReadings(char* buffer, int charsRemaining);
    int                     snprintStatusString(char* buffer, int charsRemaining, tempStatusType statusVal );
    int                     snprintStatusNode(char* buffer, int charsRemaining, tempStatusNode node);
    int                     snprintStatusTable(char* buffer, int charsRemaining);

private:
    int             _sensorType;
    tempStatusNode* _statusTable;
};

#endif