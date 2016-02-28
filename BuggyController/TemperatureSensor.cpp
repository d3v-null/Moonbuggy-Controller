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
    constructSensorNode(0, 300.0),
    constructSensorNode(255, 0.0)
};

TemperatureSensor::TemperatureSensor(){
    _sensorType = 1;
    _sensorTable = tempTable_1;
    _sensorTableLen = getSensorTableSize(_sensorTable);
}

// TemperatureSensor::setPins(int sensorPin){
//     _sensorPin = sensorPin;
// }

// void TemperatureSensor::initPins(){
//     pinMode(_sensorPin, INPUT);
// }

/**
 * Sets the sensorType, tempTable and tempTableLen 
 * if the sensorType is new and valid
 */
void TemperatureSensor::setSensorTpe(int sensorType){
    if(_sensorType != sensorType){
        switch(sensorType){
            case 1:
                _sensorTable = tempTable_1;
                break;
            default:
                return;
        }
        _sensorTableLen = getSensorTableSize(_sensorTable);
        _sensorType = sensorType;
    }
}

void TemperatureSensor::setTempBounds(double minTemp, double regTemp, double maxTemp, bool ignoreTemps){
    _minTemp =      minTemp;
    _regTemp =      regTemp;
    _maxTemp =      maxTemp;
    _ignoreTemps =  ignoreTemps;
    _tempStatusTable = {
        constructTempStatusNode(minTemp, T_NORMAL),
        constructTempStatusNode(regTemp, T_REGULATED),
        constructTempStatusNode(maxTemp, T_HOT)
    };
    _tempStatusTableLen = sizeof(_tempStatusTable) / sizeof(_tempStatusTable[0]);;
}

// void TemperatureSensor::readInputs(){
//     _sensorVal = analogRead(_sensorPin);
// }

// double TemperatureSensor::getTemp(){
//     double celsius = 0.0;
//     if(_ignoreTemps) return celsius;

//     int i;
//     for(i=1; i < _sensorTableLen; i++){
//         if (_sensorTable[i].input > _sensorVal){
//             celsius = _sensorTable[i-1].sensorVal + 
//                 (_sensorVal - _sensorTable[i].input) * 
//                 (double)(_sensorTable[i].sensorVal - _sensorTable[i-1].sensorVal)/
//                 (double)(_sensorTable[i].input - _sensorTable[i-1].input);
//             break;
//         }
//     }
//     return celsius;
// }

tempStatusNode constructTempStatusNode(double temperature, tempStatusType tempStatus){
    tempStatusNode node;
    node.temperature = temperature;
    node.tempStatus = tempStatus;
    return node;
}

tempStatusType TemperatureSensor::getTempStatus( ){
    if( ignoreTemps ) { return T_NORMAL; }
    if( tempVal < minTemp ) return T_COLD;
    tempStatusType tempStatus = T_COLD;

    int i;
    for(i=0; i < _tempStatusTableLen ; i++){
        if(tempVal >= _tempStatusTable[i].temperature){
            tempStatus = _tempStatusTable[i].tempStatus;
            break;
        }
    }
    return tempStatus;
}