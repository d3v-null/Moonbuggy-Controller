#ifndef BatterySensor_h
#define BatterySensor_h

#include "VoltageSensor.h"

typedef enum {B_LOW, B_NORMAL, B_HIGH}              batteryStatusType;
typedef struct batteryStatusNode {
    double              threshold;
    batteryStatusType   statusVal;
} batteryStatusNode;

class BatterySensor: public VoltageDividerSensor {
public:
    BatterySensor();
    void                setStatusBounds(double minBattery=0.0, double maxBattery= SYSTEM_VCC );
    batteryStatusType   getStatus();
protected:
    // double              _minBattery;
    // double              _maxBattery;
    batteryStatusNode*   _statusTable;
    // int                 _statusTableLen;
};

#endif