#ifndef BuggyController_h
#define BuggyController_h

#include "arduino.h"
#include <stdlib.h>
#include "Configuration.h"
#include "MotorController.h"

typedef enum {S_SAFE, S_TERMINATING, S_TERMINATED}  safetyStatusType;
typedef enum {TH_ZERO, TH_NORMAL, TH_BOOST}         throttleStatusType;
typedef struct throttleStatusNode {
    double threshold; 
    throttleStatusType statusVal;
} throttleStatusNode;
typedef enum {P_FORWARD, P_REVERSE}                 phaseType;
typedef enum {B_LOW, B_NORMAL, B_HIGH}              batteryStatusType;
typedef struct batteryStatusNode {
    double threshold;
    batteryStatusType statusVal;
} batteryStatusNode;

#endif