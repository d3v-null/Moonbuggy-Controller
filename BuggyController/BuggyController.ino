
#include "Configuration.h"

// Safety Stuff
typedef enum {SAFE, TERMINATING, TERMINATED} safetyStatusType;
safetyStatusType safetyStatus = SAFE; // Safety status of system, starts off as safe

// int throttleRaw; // The raw value of the throttle from the digital IO (0 -> 255)
double throttleNormalized; // The normalized value of the throttle (0.0 -> 1.0)
double 
int killSwitch;  // The value of the killswitch sensor

void readInputs() {
    // Store the value of the throttle in throttleRaw
    // Normalize this value into throttleNormalized
    // Store the value of killswitch in killSwitch;

}

/**
 * If system thinks that it is safe, performs a check of safety parameters, 
 * updates variable safetyStatus
 */
void safetyCheck() {
    switch (safetyStatus) {
        case SAFE:
          // checks killswitch value
          // checks voltages
          // checks temperatures
          // updates safetyStatus if necessary
          // terminates system if necessary
          break;
        case TERMINATING:
          // Should not be terminating
          break;
        default:
          break;
    }
}

void setup() {

}

void loop() {
    if(safety)    

}
