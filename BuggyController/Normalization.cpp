double normalize(int input, int minimum = 0, int maximum = 1024){
    if(minimum == maximum) return minimum;
    input = max(input, minimum);
    input = min(input, maximum);
    return (double)( input - minimum ) / (double)( maximum  - minimum);
}

double deNormalize(double normalizedInput, int minimum = 0, int maximum = 1024){
    if(minimum == maximum) return minimum;
    return (int)( (maximum - minimum) * normalizedInput + minimum);
}