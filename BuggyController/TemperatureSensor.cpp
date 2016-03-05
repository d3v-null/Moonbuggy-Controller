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
 * Source file for temperature sensing functions
 */

#include "TemperatureSensor.h"

sensorNode tempTable_1[] = {
    constructSensorNode(0,                      300.0),
    constructSensorNode(SYSTEM_ANALOGUE_MAX,    0.0)
};

TemperatureSensor::TemperatureSensor(){
    _sensorType = 1;
    _sensorTable = tempTable_1;
    // _sensorTableLen = SIZEOFTABLE(_sensorTable);
    setStatusBounds();
}

/**
 * Sets the sensorType, tempTable and tempTableLen 
 * if the sensorType is new and valid
 */
void TemperatureSensor::setSensorType(int sensorType){
    if(_sensorType != sensorType){
        switch(sensorType){
            case 1:
                _sensorTable = tempTable_1;
                break;
            default:
                return;
        }
        // _sensorTableLen = getSensorTableSize(_sensorTable);
        _sensorType = sensorType;
    }
}

tempStatusNode constructTempStatusNode(double threshold, tempStatusType statusVal){
    tempStatusNode node;
    node.threshold = threshold;
    node.statusVal = statusVal;
    return node;
}

void TemperatureSensor::setStatusBounds(double minTemp, double regTemp, double maxTemp){
    if(regTemp >= minTemp and maxTemp >= regTemp){
        _statusTable[0].threshold = minTemp; _statusTable[0].statusVal = T_COLD;
        _statusTable[1].threshold = regTemp; _statusTable[1].statusVal = T_NORMAL;
        _statusTable[2].threshold = maxTemp; _statusTable[2].statusVal = T_REGULATED;
    }
}

tempStatusType TemperatureSensor::getStatus( ){
    tempStatusType statusVal = T_HOT;
    int i;
    for(i=0; i < TEMPERATURE_STATUS_NODES ; i++){
        if(getSensorVal() < _statusTable[i].threshold){
            statusVal = _statusTable[i].statusVal;
            break;
        }
    }
    return statusVal;
}