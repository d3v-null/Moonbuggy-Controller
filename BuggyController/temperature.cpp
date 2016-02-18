double analogue2temp(rawValue, sensorType) {
    double celsius = 0;

    switch(sensorType){
        case 1:
            tempTable = tempTable_1;
            break;
        default:
            //sensorType not recognised
            return celsius;
            break;
    }
    tempTableLen = sizeof(tempTable) / sizeof(tempTable[0]);

    int i;
    for(i=1; i<tempTableLen; i++){
        if (tempTable[i][0] > rawValue){
            celsius = tempTable[i-1] + 
                (raw - tempTable[i][0]) * 
                (double)(tempTable[i][1] - tempTable[i-1][1])/
                (double)(tempTable[i][0] - tempTable[i-1][0])
        }
    }
}