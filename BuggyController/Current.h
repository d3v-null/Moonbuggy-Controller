#ifndef Current_h
#define Current_h 


//TODO: This table
const double currentTable_1[][2] = {
    {0, 0},
    {255, 40}
};

double analog2current(int rawValue, int sensorType);

#endif