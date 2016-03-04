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
        int i=0;
        _statusTable[i++] = constructBatteryStatusNode(minBattery, B_LOW);
        _statusTable[i++] = constructBatteryStatusNode(maxBattery, B_HIGH);
    }
}

batteryStatusType BatterySensor::getStatus(){
    batteryStatusType statusVal = B_HIGH;

    int i;
    for( i =0; i < BATTERY_STATUS_NODES ; i++){
        if(getSensorVal() >= _statusTable[i].threshold){
            statusVal = _statusTable[i].statusVal;
            break;
        }
    }
    return statusVal;
}