#include "Normalization.h"

double normalize(int input, int minimum, int maximum){
    if(minimum == maximum) return minimum;
    input = max(input, minimum);
    input = min(input, maximum);
    return (double)( input - minimum ) / (double)( maximum  - minimum);
}

double deNormalize(double normalizedInput, int minimum, int maximum){
    if(minimum == maximum) return minimum;
    return (int)( (maximum - minimum) * normalizedInput + minimum);
}