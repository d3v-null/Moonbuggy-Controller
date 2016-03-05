
#include "ThrottleSensor.h"



ThrottleSensor::ThrottleSensor(){
    setStatusBounds();
}

int ThrottleSensor::snprintStatusString(char* buffer, int charsRemaining){
    char* statusStr = "???";
    switch (getStatus()) {
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

int ThrottleSensor::snprintReadings(char* buffer, int charsRemaining){
    char*start = buffer;
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "THP:%3d|",(int)(100 * getSensorVal()) );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "THS:");
    charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "|THR:%4d|",(getRawVal()));
    return charsPrinted;

}
// throttleStatusNode constructThrottleStatusNode(double threshold, throttleStatusType statusVal){
//     throttleStatusNode node;
//     node.threshold = threshold;
//     node.statusVal = statusVal;
//     return node;
// }

void ThrottleSensor::setStatusBounds(double throttleZero, double throttleBoost){
    if(throttleZero >= 0.0 and throttleBoost > throttleZero and throttleBoost <= 1.0){
        _statusTable[0].threshold = throttleZero;
        _statusTable[0].statusVal = TH_ZERO;
        _statusTable[1].threshold = throttleBoost;
        _statusTable[1].statusVal = TH_NORMAL;
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