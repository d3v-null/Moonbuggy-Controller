#include "DigitalSmooth.h"

DigitalSmooth::DigitalSmooth(int sampleWidth, double endDiscard){
    _sampleWidth = sampleWidth;
    _samplesRaw = new int[_sampleWidth];
    _samplesSorted = new int[_sampleWidth];
    _sampleCurrent = 0;
    _sampleUsage = 0;
    _endDiscard = constrain(endDiscard, 0.0, 0.5);
}

int DigitalSmooth::smoothData(int dataPoint){
    _samplesRaw[_sampleCurrent] = dataPoint;
    _sampleCurrent = (_sampleCurrent + 1) % _sampleWidth;
    _sampleUsage = max(_sampleUsage, _sampleCurrent);
    for(int i=0; i<_sampleUsage; i++){
        _samplesSorted[i] = _samplesRaw[i];
    }

    if(_sampleUsage < 2){
      return dataPoint;
    } else {
      quickSortInt(_samplesSorted, _sampleUsage);   
      int discard = (int)(_endDiscard * _sampleUsage);
      int end = _sampleUsage - 1 - discard;
      if(end - discard < 1){
        return dataPoint;
      } else {
        int total = 0;
        for( int i=discard; i<end; i++ ){
          total += _samplesSorted[i];
        }
        return (int)( total / (end - discard) );

      }
    }



}

int DigitalSmooth::snprintSamples(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "{" );
    for(int i=0; i<_sampleUsage; i++){
      charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "%3d,", _samplesSorted[i] );
    }
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "}" );
    return charsPrinted;
}

void DigitalSmooth::quickSortInt(int *ar, int n) 
{
  if (n < 2)
    return;
  int p = ar[n / 2];
  int *l = ar;
  int *r = ar + n - 1;
  while (l <= r) {
    if (*l < p) {
      l++;
    }
    else if (*r > p) {
      r--;
    }
    else {
      int t = *l;
      *l = *r;
      *r = t;
      l++;
      r--;
    }
  }
  quickSortInt(ar, r - ar + 1);
  quickSortInt(l, ar + n - l);
}