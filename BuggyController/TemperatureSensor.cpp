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

int rawVals_1[]       = {0,     SYSTEM_ANALOGUE_MAX};
double sensorVals_1[] = {300.0, 0.0};
int len_1 = min(ARRAYLEN(rawVals_1), ARRAYLEN(sensorVals_1));

TemperatureSensor::TemperatureSensor(){
    setStatusBounds();
    populateSensorTable(len_1, rawVals_1, sensorVals_1);
}

/**
 * Sets the sensorType, tempTable and tempTableLen 
 * if the sensorType is new and valid
 */
void TemperatureSensor::setSensorType(int sensorType){
    if(_sensorType != sensorType){
        int* rawVals;
        double* sensorVals;
        int len = 0;

        switch(sensorType){
            case 1:
                rawVals = rawVals_1;
                sensorVals = sensorVals_1;
                len = len_1;
                break;
            default:
                return;
        }
        populateSensorTable(len, rawVals, sensorVals);
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
    double sensorVal = getSensorVal();
    int i;
    for(i=0; i < TEMPERATURE_STATUS_NODES ; i++){
        if(sensorVal < _statusTable[i].threshold){
            statusVal = _statusTable[i].statusVal;
            break;
        }
    }
    return statusVal;
}


int TemperatureSensor::snprintStatusString(char* buffer, int charsRemaining, tempStatusType statusVal ){
    char* statusStr = "???";
    switch ( statusVal ) {
        case T_HOT:
          statusStr = "HOT";
          break;
        case T_COLD:
          statusStr = "CLD";
          break;
        case T_NORMAL:
          statusStr = "NRM";
          break;
        case T_REGULATED:
          statusStr = "REG";
          break;
        default:
          break;
    }
    return snprintf(buffer, charsRemaining, statusStr);
}

int TemperatureSensor::snprintStatusNode(char* buffer, int charsRemaining, tempStatusNode node){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "{%4d:", (int)(node.threshold) );
    charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), node.statusVal );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "}" );
    return charsPrinted;
}

int TemperatureSensor::snprintStatusTable(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "[");
    int i;
    for( i=0; i < TEMPERATURE_STATUS_NODES; i++) {
        charsPrinted += snprintStatusNode((buffer+charsPrinted), abs(charsRemaining-charsPrinted), _statusTable[i]);
    }
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "]");
    return charsPrinted;
}

int TemperatureSensor::snprintReadings(char* buffer, int charsRemaining){
    char*start = buffer;
    int charsPrinted = 0;
    charsPrinted += snprintSensorTable((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "PER:%3d|",(int)(100 * getSensorVal()) );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "STS:");
    charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), getStatus());
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "|RAW:%4d",(getRawVal()));
    return charsPrinted;
}