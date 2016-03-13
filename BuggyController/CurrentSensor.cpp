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

// sensorNode currentTable_1[] = {
//     constructSensorNode(0,                      0.0),
//     constructSensorNode(SYSTEM_ANALOGUE_MAX,    40.0)
// };

CurrentSensor::CurrentSensor(){
    setStatusBounds();
    // _sensorTable = currentTable_1;
}

void CurrentSensor::initSensorTable(){
    int rawVals_1[]       = {0,     512,  540, 580, SYSTEM_ANALOGUE_MAX};
    double sensorVals_1[] = {-40.0, 0.0, 3.05,  10, 40.0};
    int len_1 = min(ARRAYLEN(rawVals_1), ARRAYLEN(sensorVals_1));
    populateSensorTable(len_1, rawVals_1, sensorVals_1);
    // setSensorMultiplier();


    //10 -> 580
    //3.05 -> 540

}

// void CurrentSensor::setSensorType(int sensorType){
//     if(_sensorType != sensorType){
//         switch(sensorType){
//             case 1:
//                 _sensorTable = currentTable_1;
//                 break;
//             default:
//                 return;
//         }
//         _sensorType = sensorType;
//     }
// }

// currentStatusNode constructCurrentStatusNode(double threshold, currentStatusType statusVal){
//     currentStatusNode node;
//     node.threshold = threshold;
//     node.statusVal = statusVal;
//     return node;
// }

void CurrentSensor::setStatusBounds(double currentReg, double currentMax){
    if(currentReg >= 0.0 and currentMax > currentReg){
        double thresholds[] =           {currentReg,  currentMax};
        currentStatusType statuses[]  = {C_NORMAL,    C_REGULATED};
        for(int i=0; i< _statusNodes; i++){
            _statusTable[i].threshold = thresholds[i];
            _statusTable[i].statusVal = statuses[i];
        }
    }
}

currentStatusType CurrentSensor::getStatus(){
    currentStatusType statusVal = C_HIGH;
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

int CurrentSensor::snprintStatusString(char* buffer, int charsRemaining, currentStatusType statusVal){
    char* statusStr = "???";
    switch (statusVal) {
        case C_NORMAL:
          statusStr = "NOR";
          break;
        case C_REGULATED:
          statusStr = "REG";
          break;
        case C_HIGH:
          statusStr = "HI ";
          break;
        default:
          break;
    }
    return snprintf(buffer, charsRemaining, statusStr);
}

int CurrentSensor::snprintStatusNode(char* buffer, int charsRemaining, currentStatusNode node){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "{%4d:", (int)(node.threshold) );
    charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), node.statusVal );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "}" );
    return charsPrinted;
}

int CurrentSensor::snprintStatusTable(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "[");
    for(int i=0; i < _statusNodes; i++) {
        charsPrinted += snprintStatusNode((buffer+charsPrinted), abs(charsRemaining-charsPrinted), _statusTable[i]);
    }
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "]");
    return charsPrinted;
}

int CurrentSensor::snprintReadings(char* buffer, int charsRemaining){
    char*start = buffer;
    int charsPrinted = 0;
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "SNT@%d:", (int)(_sensorTable)%1000);
    // charsPrinted += snprintSensorTable((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "AMP:");
    charsPrinted += strlen( dtostrf(getSensorVal(), FLOAT_WIDTH, FLOAT_PREC, (buffer+charsPrinted))) ;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "|STS:");
    charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), getStatus());
    #ifdef CALLIBRATE_SENSORS
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "|RAW:%4d",(getRawVal()));
    #endif
    return charsPrinted;
}