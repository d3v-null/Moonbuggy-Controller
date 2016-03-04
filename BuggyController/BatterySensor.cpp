#include "BatterySensor.h"

batteryStatusNode constructBatteryStatusNode(double threshold, batteryStatusType statusVal){
    batteryStatusNode node;
    node.threshold = threshold;
    node.statusVal = statusVal;
    return node;
}

BatterySensor::BatterySensor(){
    setStatusBounds();
}

void BatterySensor::setStatusBounds(double minBattery, double maxBattery ){
    if( minBattery >= 0.0 and maxBattery >= minBattery ){
        // _minBattery = minBattery;
        // _maxBattery = maxBattery;
        _statusTable = {
            constructBatteryStatusNode(minBattery, B_LOW),
            constructBatteryStatusNode(maxBattery, B_HIGH)
        };
        // _statusTableLen = SIZEOFTABLE(_statusTable) ; 
    }
}

batteryStatusType BatterySensor::getStatus(){
    batteryStatusType statusVal = B_HIGH;

    int i;
    for(i=0; i < SIZEOFTABLE(_statusTable) ; i++){
        if(getSensorVal() >= _statusTable[i].threshold){
            statusVal = _statusTable[i].statusVal;
            break;
        }
    }
    return statusVal;
}