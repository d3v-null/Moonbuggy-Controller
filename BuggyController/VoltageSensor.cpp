#include "VoltageSensor.h"

sensorNode constructSensorNode(int input, double sensorVal){
    sensorNode node;
    node.input = input;
    node.sensorVal = sensorVal;
    return node;
}

VoltageSensor::VoltageSensor() {
    _pinsSet = false;
    _pinsInit = false;
    _smootherInit = false;
    _rawVal = 0;
    _smoothedVal = 0;
};

void VoltageSensor::initSensorTable(){
    int rawVals[] =         {0,     SYSTEM_ANALOGUE_MAX};
    double sensorVals[] =   {0.0,   SYSTEM_VCC};
    int len = min(ARRAYLEN(rawVals), ARRAYLEN(sensorVals));
    populateSensorTable(len, rawVals, sensorVals);
}

void VoltageSensor::initSmoother(){
    _smoother = new DigitalSmooth();
    _smootherInit = true;
}

void VoltageSensor::init(){
    initSensorTable();
    #ifdef ENABLE_SMOOTHING
        initSmoother();
    #endif
}

VoltageSensor::~VoltageSensor(){
    return;
}

void VoltageSensor::populateSensorTable(int len, int rawVals[], double sensorVals[] ){
    _sensorTableLen = len;
    _sensorTable = new sensorNode[len];
    int i;
    for(i=0; i<len; i++){
        _sensorTable[i].input = rawVals[i];
        _sensorTable[i].sensorVal = sensorVals[i];
    }
}

void VoltageSensor::setPins( int sensorPin ){
    _sensorPin = sensorPin;
    _pinsSet = true;
}

void VoltageSensor::initPins(){
    if(_pinsSet){
        pinMode(_sensorPin, INPUT);
        _pinsInit = true;
    }
}

void VoltageSensor::readInputs(){
    if(_pinsInit){
        _rawVal =  analogRead(_sensorPin);
        if(_smootherInit){
            _smoothedVal = _smoother->smoothData( _rawVal );
        } else {
            _smoothedVal = _rawVal;
        }
    }
}

int VoltageSensor::snprintSensorNode(char* buffer, int charsRemaining, sensorNode node){
    return snprintf(buffer, charsRemaining, "{%4d:%4d}", node.input, (int)(node.sensorVal * 100));
}

int VoltageSensor::snprintSensorTable(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    int i;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "@%d", (int)(_sensorTable)%1000);
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "[");
    for(i=0; i < _sensorTableLen ; i++){
        charsPrinted += snprintSensorNode((buffer+charsPrinted), abs(charsRemaining-charsPrinted), _sensorTable[i]);
    }
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "]");
    return charsPrinted;
}

int VoltageSensor::snprintSensorVal(char* buffer, int charsRemaining){
    double sensorVal = getSensorVal();
    int digits = 1;
    if(abs(sensorVal) > 1) { int digits = (int)(log10(abs(sensorVal))); }
    if(sensorVal < 0){ digits++; }
    if(digits + 1 >= DEBUG_CELL_WIDTH){ 
        return snprintf( buffer, charsRemaining, DEBUG_CELL_FMT_D, (int)(sensorVal) ); 
    } else {
        int float_prec = DEBUG_CELL_WIDTH - digits - 1;
        float_prec = constrain(float_prec, 0, FLOAT_PREC);
        dtostrf(sensorVal, FLOAT_WIDTH, float_prec, buffer);
        return DEBUG_CELL_WIDTH;
    }
    // if(digits == 0){float_width = 0; }
}

int VoltageSensor::snprintValLabel(char* buffer, int charsRemaining){
    return snprintf(buffer, charsRemaining, "VAL:");
}

int VoltageSensor::snprintReadings(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    // #ifndef DATA_LOGGING
    //     charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "SNT@%d:", (int)(_sensorTable)%1000);
    // #endif
    // charsPrinted += snprintSensorTable((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    #ifndef ENABLE_NORMALIZATION
        #ifndef DATA_LOGGING
            charsPrinted += snprintValLabel((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
        #endif
        charsPrinted += snprintSensorVal((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
        #ifdef CALLIBRATE_SENSORS
            #ifndef DATA_LOGGING
                charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "RAW:");
            #endif
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_CELL_FMT_D,(getRawVal()));
            charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
            // #ifndef DATA_LOGGING
            //     charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "SMR:");
            // #endif
            // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_CELL_FMT_D,(getSmoothedVal()));
            // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
        #endif
    #endif
    return charsPrinted - 1;
}

// int VoltageSensor::getSensorTableSize(){
//     return _sensorTableLen ;
// }

/** 
 * Overridden in later classes
 */
int VoltageSensor::getRawVal(){
    return _rawVal;
}

int VoltageSensor::getSmoothedVal(){
    return _smoothedVal;
}

double VoltageSensor::getSensorVal(){
    double sensorVal = 0.0;
    int rawVal = getSmoothedVal();
    int i;
    for(i=1; i < _sensorTableLen ; i++){
        if (_sensorTable[i].input >= rawVal){
            /* y = mx + c */
            sensorVal = _sensorTable[i-1].sensorVal + 
                (rawVal - _sensorTable[i-1].input) * 
                (double)(_sensorTable[i].sensorVal - _sensorTable[i-1].sensorVal)/
                (double)(_sensorTable[i].input - _sensorTable[i-1].input);
            break;
        }
    }
    return sensorVal;
}

// NormalizedVoltageSensor::NormalizedVoltageSensor() {}

void NormalizedVoltageSensor::initSensorTable(){
    setInputConstraints(0,  SYSTEM_ANALOGUE_MAX);
}

int NormalizedVoltageSensor::getSmoothedVal(){
    int rawVal = VoltageSensor::getSmoothedVal();
    return constrain(
        rawVal, 
        _sensorMin, 
        _sensorMax
    );
}

// int NormalizedVoltageSensor::getSensorMin() {return _sensorMin;}
// int NormalizedVoltageSensor::getSensorMax() {return _sensorMax;}

void NormalizedVoltageSensor::setInputConstraints(int minimum, int maximum) {
    if(minimum >= 0 and maximum >= minimum and maximum <= SYSTEM_ANALOGUE_MAX){
        _sensorMin = minimum;
        _sensorMax = maximum;
        int rawVals[] =         {minimum,    maximum};
        double sensorVals[] =   {0.0,        1.0};
        int len = min(ARRAYLEN(rawVals), ARRAYLEN(sensorVals));
        populateSensorTable(len, rawVals, sensorVals);
    }
}

// VoltageDividerSensor::VoltageDividerSensor(){}

void VoltageDividerSensor::initSensorTable(){
    setSensorMultiplier();
}

void VoltageDividerSensor::setSensorMultiplier(double sensorMultiplier){
    if(sensorMultiplier > 0.0){
        _sensorMultiplier = sensorMultiplier;
        int rawVals[] =         {0,   SYSTEM_ANALOGUE_MAX};
        double sensorVals[] =   {0.0, sensorMultiplier * SYSTEM_VCC};
        int len = min(ARRAYLEN(rawVals), ARRAYLEN(sensorVals));
        populateSensorTable(len, rawVals, sensorVals);
    }
}
