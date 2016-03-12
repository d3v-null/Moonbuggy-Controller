#ifndef BatterySensor_h
#define BatterySensor_h

#include "VoltageSensor.h"

typedef enum {B_LOW, B_NORMAL, B_HIGH}              batteryStatusType;
typedef struct batteryStatusNode {
    double              threshold;
    batteryStatusType   statusVal;
} batteryStatusNode;

#define BATTERY_STATUS_NODES 2 

class BatterySensor: public VoltageDividerSensor {
public:
    BatterySensor();
    void                setStatusBounds(double minBattery=0.0, double maxBattery= SYSTEM_VCC );
    batteryStatusType   getStatus();
    // DEBUG
    int                 snprintStatusNode(char* buffer, int charsRemaining, batteryStatusNode node);
    int                 snprintStatusTable(char* buffer, int charsRemaining);
    int                 snprintReadings(char* buffer, int charsRemaining);
    int                 snprintStatusString(char* buffer, int charsRemaining, batteryStatusType statusVal);
protected:
    // double              _minBattery;
    // double              _maxBattery;
    batteryStatusNode   _statusTable[BATTERY_STATUS_NODES];
    // int                 _statusTableLen;
};

#endif