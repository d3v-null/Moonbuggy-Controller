#ifndef BuggyController_h
#define BuggyController_h

#include "arduino.h"
#include <stdlib.h>
#include "Configuration.h"
#include "ThrottleSensor.h"
#include "MotorController.h"
#include <Math.h>

#ifndef IGNORE_TEMPS
    #include "TemperatureSensor.h"
#endif
#ifndef IGNORE_BATTERY
    #include "BatterySensor.h"
#endif
#ifndef IGNORE_CURRENTS
    #include "CurrentSensor.h"
    // #ifndef DEBUG
    // #endif
#endif

typedef enum {S_INITIAL, S_SAFE, S_TERMINATING, S_TERMINATED}  safetyStatusType;
typedef enum {P_FORWARD, P_REVERSE}                 phaseType;

void printDebugInfo();

int snprintVehicleModeString(char* buffer, int charsRemaining, motorModeType modeVal );
int snprintPhaseString(char* buffer, int charsRemaining, phaseType phase );
int snprintSafetyStatusString(char* buffer, int charsRemaining, safetyStatusType statusVal );
char* digitalStatusString(int digitalValue);

#endif