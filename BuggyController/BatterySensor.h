#ifndef BatterySensor_h
#define BatterySensor_h

#include "VoltageSensor.h"

typedef enum {B_LOW, B_NORMAL, B_HIGH}              batteryStatusType;
typedef struct batteryStatusNode {
    double              threshold;
    batteryStatusType   statusVal;
} batteryStatusNode;

// #define BATTERY_STATUS_NODES 2 

class BatterySensor: public VoltageDividerSensor {
public:
    BatterySensor();
    void                setStatusBounds(double minBattery=0.0, double maxBattery= SYSTEM_VCC );
    batteryStatusType   getStatus();
    void                    initSmoother();
    // DEBUG
    int             snprintNormalized(char* buffer, int charsRemaining);
    int                 snprintStatusNode(char* buffer, int charsRemaining, batteryStatusNode node);
    int                 snprintStatusTable(char* buffer, int charsRemaining);
    int                 snprintReadings(char* buffer, int charsRemaining);
    int                 snprintValLabel(char* buffer, int charsRemaining);
    int                 snprintStatusString(char* buffer, int charsRemaining, batteryStatusType statusVal);
protected:
    double              _minVal;
    double              _maxVal;
    const static int        _statusNodes = 2;
    batteryStatusNode      _statusTable[_statusNodes];
    // int                 _statusTableLen;
};

#endif