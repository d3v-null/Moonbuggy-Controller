/*
    Shunt wound motor control firmware 

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Source file for current sensing functions
 */

#include "CurrentSensor.h"

sensorNode currentTable_1[] = {
    constructSensorNode(0,                      0.0),
    constructSensorNode(SYSTEM_ANALOGUE_MAX,    40.0)
};

CurrentSensor::CurrentSensor(){
    _sensorType = 1;
    _sensorTable = currentTable_1;
}

void CurrentSensor::setSensorType(int sensorType){
    if(_sensorType != sensorType){
        switch(sensorType){
            case 1:
                _sensorTable = currentTable_1;
                break;
            default:
                return;
        }
        _sensorType = sensorType;
    }
}

currentStatusNode constructCurrentStatusNode(double threshold, currentStatusType statusVal){
    currentStatusNode node;
    node.threshold = threshold;
    node.statusVal = statusVal;
    return node;
}

void CurrentSensor::setStatusBounds( double currentReg, double currentMax){
    if(currentReg >= 0.0 and currentMax <= 1.0){
        _currentStatusTable = {
            constructCurrentStatusNode(currentReg, A_NORMAL),
            constructCurrentStatusNode(currentMax, A_REGULATED)
        }
    }
}

currentStatusType CurrentSensor::getStatus(){
    currentStatusType statusVal = C_HIGH;
    int i;
    for(i=0; i < SIZEOFTABLE(_currentStatusTable) ; i++){
        if(getSensorVal() < _currentStatusTable[i].threshold){
            statusVal = _currentStatusTable[i].statusVal;
            break;
        }
    }
    return statusVal;
}
