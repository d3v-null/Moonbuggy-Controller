#ifndef DigitalSmooth_h
#define DigitalSmooth_h

#include "arduino.h"

class DigitalSmooth
{
public:
    DigitalSmooth(int sampleWidth = 10, double endDiscard = 0.0);
    ~DigitalSmooth();
    int smoothData(int dataPoint);
    int snprintSamples(char* buffer, int charsRemaining);
private:
    void quickSortInt(int *ar, int n) ;
    int*  _samplesRaw;
    int*  _samplesSorted;
    double _endDiscard;
    int _sampleWidth;
    int _sampleCurrent;
    int _sampleUsage;
};
#endif