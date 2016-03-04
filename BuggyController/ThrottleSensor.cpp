
#include "ThrottleSensor.h"



ThrottleSensor::ThrottleSensor(){
    setStatusBounds();
}

throttleStatusNode constructThrottleStatusNode(double threshold, throttleStatusType statusVal){
    throttleStatusNode node;
    node.threshold = threshold;
    node.statusVal = statusVal;
    return node;
}

void ThrottleSensor::setStatusBounds(double throttleZero, double throttleBoost){
    if(throttleZero >= 0.0 and throttleBoost > throttleZero and throttleBoost <= 1.0){
        // _throttleZero   = throttleZero;
        // _throttleBoost  = throttleBoost;
        _statusTable = {
            constructThrottleStatusNode(_throttleZero,  TH_ZERO),
            constructThrottleStatusNode(_throttleBoost, TH_NORMAL)
        };
        // _statusTableLen = SIZEOFTABLE(_statusTable) ;
    }
}

throttleStatusType ThrottleSensor::getStatus(){
    throttleStatusType statusVal = TH_BOOST;
    int i;
    for( i=0; i < SIZEOFTABLE(_statusTable); i++) {
        if(getSensorVal() < _statusTable[i].threshold){
            statusVal = _statusTable[i].statusVal;
            break;
        } 
    }
    return statusVal;
}