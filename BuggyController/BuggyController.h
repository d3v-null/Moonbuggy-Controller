#ifndef BuggyController_h
#define BuggyController_h

#include "arduino.h"
#include <stdlib.h>
#include "Configuration.h"
#include "MotorController.h"
#include "TemperatureSensor.h"
#include "ThrottleSensor.h"
#include "BatterySensor.h"

typedef enum {S_SAFE, S_TERMINATING, S_TERMINATED}  safetyStatusType;
typedef enum {P_FORWARD, P_REVERSE}                 phaseType;

void printDebugInfo();

#endif