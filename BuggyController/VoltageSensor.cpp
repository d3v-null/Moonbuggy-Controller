#include "VoltageSensor.h"

sensorNode constructSensorNode(int input, double sensorVal){
    sensorNode node;
    node.input = input;
    node.sensorVal = sensorVal;
    return node;
}

// int getSensorTableSize(sensorNode* sensorTable){
//     return sizeof(sensorTable) / sizeof(sensorTable[0]);
// }

VoltageSensor::VoltageSensor() {
    // Serial.println("VoltageSensor Constructor"); delay(100);
    // _sensorPin = -1;
    _pinsSet = false;
    _pinsInit = false;
    _smootherInit = false;
    _rawVal = 0;
    // initSensorTable();
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
    initSmoother();
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
            _rawVal = _smoother->smoothData( _rawVal );
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

int VoltageSensor::getSensorTableSize(){
    return _sensorTableLen ;
}

/** 
 * Overridden in later classes
 */
int VoltageSensor::getRawVal(){
    return _rawVal;
}

double VoltageSensor::getSensorVal(){
    double sensorVal = 0.0;
    int rawVal = getRawVal();
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

NormalizedVoltageSensor::NormalizedVoltageSensor() {}

void NormalizedVoltageSensor::initSensorTable(){
    setInputConstraints(0,  SYSTEM_ANALOGUE_MAX);
}

int NormalizedVoltageSensor::getRawVal(){
    int rawVal = VoltageSensor::getRawVal();
    return constrain(
        rawVal, 
        _sensorMin, 
        _sensorMax
    );
}

int NormalizedVoltageSensor::getSensorMin() {return _sensorMin;}
int NormalizedVoltageSensor::getSensorMax() {return _sensorMax;}

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

VoltageDividerSensor::VoltageDividerSensor(){}

void VoltageDividerSensor::initSensorTable(){
    setSensorMultiplier();
}

void VoltageDividerSensor::setSensorMultiplier(double sensorMultiplier){
    if(sensorMultiplier > 0.0){
        int rawVals[] =         {0,   SYSTEM_ANALOGUE_MAX};
        double sensorVals[] =   {0.0, sensorMultiplier * SYSTEM_VCC};
        int len = min(ARRAYLEN(rawVals), ARRAYLEN(sensorVals));
        populateSensorTable(len, rawVals, sensorVals);
    }
}
