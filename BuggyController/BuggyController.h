#ifndef BuggyController_h
#define BuggyController_h

#include "arduino.h"
#include <stdlib.h>
#include "Configuration.h"
#include "ThrottleSensor.h"
#include "MotorController.h"
#ifndef IGNORE_TEMPS
    #include "TemperatureSensor.h"
#endif
#ifndef IGNORE_BATTERY
    #include "BatterySensor.h"
#endif

typedef enum {S_SAFE, S_TERMINATING, S_TERMINATED}  safetyStatusType;
typedef enum {P_FORWARD, P_REVERSE}                 phaseType;

void printDebugInfo();

#endif