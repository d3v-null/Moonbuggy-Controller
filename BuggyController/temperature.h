#ifndef Temperature_h
#define Temperature_h 

const double tempTable_1[][2] = {
    {0, 300},
    {255, 0}
};

double analog2temp(int rawValue, int sensorType);

#endif