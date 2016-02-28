#ifndef Normalization_h
#define Normalization_h

double normalize(int input, int minimum = 0, int maximum = 1024);
double deNormalize(double normalizedInput, int minimum = 0, int maximum = 1024);

#endif