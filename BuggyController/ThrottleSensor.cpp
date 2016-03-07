
#include "ThrottleSensor.h"



ThrottleSensor::ThrottleSensor(){
    setStatusBounds();
}

void ThrottleSensor::setStatusBounds(double throttleZero, double throttleBoost){
    if(throttleZero >= 0.0 and throttleBoost > throttleZero and throttleBoost <= 1.0){
        double thresholds[] =       {throttleZero,  throttleBoost};
        throttleStatusType statuses[] = {TH_ZERO,       TH_NORMAL};
        for(int i=0; i<THROTTLE_STATUS_NODES; i++){
            _statusTable[i].threshold = thresholds[i];
            _statusTable[i].statusVal = statuses[i];
        }
    }
}

throttleStatusType ThrottleSensor::getStatus(){
    throttleStatusType statusVal = TH_BOOST;
    double sensorVal = getSensorVal();
    int i;
    for( i=0; i < THROTTLE_STATUS_NODES; i++) {
        if(sensorVal < _statusTable[i].threshold){
            statusVal = _statusTable[i].statusVal;
            break;
        } 
    }
    return statusVal;
}

int ThrottleSensor::snprintStatusString(char* buffer, int charsRemaining, throttleStatusType statusVal){
    char* statusStr = "???";
    switch (statusVal) {
        case TH_ZERO:
          statusStr = "ZER";
          break;
        case TH_NORMAL:
          statusStr = "NOR";
          break;
        case TH_BOOST:
          statusStr = "BST";
          break;
        default:
          break;
    }
    return snprintf(buffer, charsRemaining, statusStr);
}

int ThrottleSensor::snprintStatusNode(char* buffer, int charsRemaining, throttleStatusNode node){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "{%4d:", (int)(100 * node.threshold) );
    charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), node.statusVal );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "}" );
    return charsPrinted;
}

int ThrottleSensor::snprintStatusTable(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "[");
    int i;
    for( i=0; i < THROTTLE_STATUS_NODES; i++) {
        charsPrinted += snprintStatusNode((buffer+charsPrinted), abs(charsRemaining-charsPrinted), _statusTable[i]);
    }
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "]");
    return charsPrinted;
}

int ThrottleSensor::snprintReadings(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "PIN:%3d|",(int)(_sensorPin) );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "SNT@%d:", (int)(_sensorTable)%1000);
    charsPrinted += snprintSensorTable((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "STT:");
    // charsPrinted += snprintStatusTable((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "PER:%3d|",(int)(100 * getSensorVal()) );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "STS:");
    charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), getStatus());
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "|RAW:%4d|",(getRawVal()));
    return charsPrinted;
}