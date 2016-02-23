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

 #include "Current.h"

double analog2current(int rawValue, int sensorType) {
    double current = 0.0;

    switch(sensorType){
        case 1:
            const double currentTable[][2] = currentTable_1;
            break;
        default:
            //sensorType not recognised
            return current;
            break;
    }
    currentTableLen = sizeof(currentTable) / sizeof(currentTable[0]);

    int i;
    for(i=1; i<currentTableLen; i++){
        if (currentTable[i][0] > rawValue){
            current = currentTable[i-1] + 
                (raw - currentTable[i][0]) * 
                (double)(currentTable[i][1] - currentTable[i-1][1])/
                (double)(currentTable[i][0] - currentTable[i-1][0])
        }
    }
}