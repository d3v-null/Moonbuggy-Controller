#include "BatterySensor.h"

BatterySensor::BatterySensor(){
    setStatusBounds();
}

void BatterySensor::setStatusBounds(double minBattery, double maxBattery ){
    if( minBattery >= 0.0 and maxBattery >= minBattery ){
        double thresholds[] = {minBattery, maxBattery};
        batteryStatusType statuses[] = {B_LOW, B_NORMAL};
        for(int i=0; i<_statusNodes; i++){
            _statusTable[i].threshold = thresholds[i];
            _statusTable[i].statusVal = statuses[i];
        }
    }
}

batteryStatusType BatterySensor::getStatus(){
    batteryStatusType statusVal = B_HIGH;
    double sensorVal = getSensorVal();
    int i;
    for( i=0; i < _statusNodes; i++) {
        if(sensorVal < _statusTable[i].threshold){
            statusVal = _statusTable[i].statusVal;
            break;
        } 
    }
    return statusVal;
}


void BatterySensor::initSmoother(){
    _smoother = new DigitalSmooth(10, 0.15);
    _smootherInit = true;
}

int BatterySensor::snprintStatusString(char* buffer, int charsRemaining, batteryStatusType statusVal){
    char* statusStr = "???";
    switch (statusVal) {
        case B_LOW:
          statusStr = "LOW";
          break;
        case B_NORMAL:
          statusStr = "NOR";
          break;
        case B_HIGH:
          statusStr = "HI ";
          break;
        default:
          break;
    }
    return snprintf(buffer, charsRemaining, statusStr);
}

int BatterySensor::snprintStatusNode(char* buffer, int charsRemaining, batteryStatusNode node){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "{%4d:", (int)(100 * node.threshold) );
    charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), node.statusVal );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "}" );
    return charsPrinted;
}

int BatterySensor::snprintStatusTable(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "[");
    int i;
    for( i=0; i < _statusNodes; i++) {
        charsPrinted += snprintStatusNode((buffer+charsPrinted), abs(charsRemaining-charsPrinted), _statusTable[i]);
    }
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "]");
    return charsPrinted;
}

int BatterySensor::snprintReadings(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "PIN:%3d|",(int)(_sensorPin) );
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "SNT@%d:", (int)(_sensorTable)%1000);
    // charsPrinted += snprintSensorTable((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "STT:");
    // charsPrinted += snprintStatusTable((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    #ifndef DATA_LOGGING
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "VLT:" );
    #endif
    charsPrinted += strlen( dtostrf(getSensorVal(), FLOAT_WIDTH, FLOAT_PREC, (buffer+charsPrinted))) ;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    #ifndef DATA_LOGGING
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "STS:");
        charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), getStatus());
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    #endif
    #ifdef CALLIBRATE_SENSORS
        #ifndef DATA_LOGGING
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "RAW:");
        #endif
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_CELL_FMT_D,(getRawVal()));
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    #endif
    return charsPrinted - 1;
}