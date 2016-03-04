#ifndef ThrottleSensor_h
#define ThrottleSensor_h

#include "VoltageSensor.h"

typedef enum {TH_ZERO, TH_NORMAL, TH_BOOST}         throttleStatusType;
typedef struct throttleStatusNode {
    double              threshold; 
    throttleStatusType  statusVal;
} throttleStatusNode;

class ThrottleSensor: public NormalizedVoltageSensor
{
public:
    ThrottleSensor();
    void                    setStatusBounds(double throttleZero=0.0, double throttleBoost=1.0);
    throttleStatusType      getStatus();
protected:
    // double                  _throttleZero;
    // double                  _throttleBoost;
    throttleStatusNode*     _statusTable;
    // int                     _statusTableLen;
};

#endif