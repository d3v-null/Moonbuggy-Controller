
#include "ThrottleSensor.h"



ThrottleSensor::ThrottleSensor(){
    setStatusBounds();
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