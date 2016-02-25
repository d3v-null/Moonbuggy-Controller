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

tempNode tempTable_1[] = {
    constructTempNode(0, 300.0),
    constructTempNode(255, 0.0)
};

tempNode constructTempNode(int input, double temperature){
    tempNode node;
    node.input = input;
    node.temperature = temperature;
    return node;
}

int getTempTableSize(tempNode* tempTable){
    return sizeof(tempTable) / sizeof(tempTable[0]);
}

double TemperatureSensor::analog2temp(int rawValue, int sensorType) {
    double celsius = 0.0;
    tempNode* tempTable;
    int tempTableLen;

    switch(sensorType){
        case 1:
            tempTable = tempTable_1;
            tempTableLen = getTempTableSize(tempTable_1);
            break;
        default:
            //sensorType not recognised
            return celsius;
            break;
    }

    int i;
    for(i=1; i<tempTableLen; i++){
        if (tempTable[i].input > rawValue){
            celsius = tempTable[i-1].temperature + 
                (rawValue - tempTable[i].input) * 
                (double)(tempTable[i].temperature - tempTable[i-1].temperature)/
                (double)(tempTable[i].input - tempTable[i-1].input);
            break;
        }
    }
    return celsius;
}

tempStatusNode constructTempStatusNode(double temperature, tempStatusType tempStatus){
    tempStatusNode node;
    node.temperature = temperature;
    node.tempStatus = tempStatus;
    return node;
}

tempStatusType TemperatureSensor::getTempStatus(double tempVal, double minTemp, double regTemp, double maxTemp, bool ignoreTemps){
    if( ignoreTemps ) { return T_NORMAL; }
    if(tempVal < minTemp) return T_COLD;
    tempStatusType tempStatus = T_COLD;
    tempStatusNode statuses[] = {
        constructTempStatusNode(minTemp, T_NORMAL),
        constructTempStatusNode(regTemp, T_REGULATED),
        constructTempStatusNode(maxTemp, T_HOT)
    };   
    int statusLen = sizeof(statuses) / sizeof(statuses[0]);
    int i;
    for(i=0; i<statusLen; i++){
        if(tempVal >= statuses[i].temperature){
            tempStatus = statuses[i].tempStatus;
            break;
        }
    }
    return tempStatus;
}