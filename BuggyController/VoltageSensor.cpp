
#include "VoltageSensor.h"

sensorNode constructSensorNode(int input, double sensorVal){
    sensorNode node;
    node.input = input;
    node.sensorVal = sensorVal;
    return node;
}

int getSensorTableSize(sensorNode* sensorTable){
    return sizeof(sensorTable) / sizeof(sensorTable[0]);
}

VoltageSensor::VoltageSensor() {
    _pinsSet = false;
    _pinsInit = false;
    _sensorTable = {
        constructSensorNode(0,                      0.0),
        constructSensorNode(SYSTEM_ANALOGUE_MAX,    SYSTEM_VCC)
    };
    _sensorTableLen = getSensorTableSize(_sensorTable);
};

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
        _rawVal = analogRead(_sensorPin);
    }
}

int VoltageSensor::getRawValue(){
    if(_sensorRead){
        return _rawVal;
    } else {
        return 0;
    }
}

double VoltageSensor::getSensorVal(){
    double sensorVal = 0.0;
    int i;
    for(i=1; i < _sensorTableLen; i++){
        if (_sensorTable[i].input > _rawVal){
            celsius = _sensorTable[i-1].sensorVal + 
                (_sensorVal - _sensorTable[i].input) * 
                (double)(_sensorTable[i].sensorVal - _sensorTable[i-1].sensorVal)/
                (double)(_sensorTable[i].input - _sensorTable[i-1].input);
            break;
        }
    }
    return sensorVal;
}

NormalizedVoltageSensor::NormalizedVoltageSensor() {
    _sensorTable = {
        constructSensorNode(0,                      0.0),
        constructSensorNode(SYSTEM_ANALOGUE_MAX,    1.0)
    };
    _sensorTableLen = getSensorTableSize(_sensorTable); 
}

VoltageDividerSensor::VoltageDividerSensor() {
    //stub constructor?
}

void VoltageDividerSensor::setSensorMultiplier(double sensorMultiplier){
    _sensorTable = {
        constructSensorNode(0,                      0.0),
        constructSensorNode(SYSTEM_ANALOGUE_MAX,    sensorMultiplier * SYSTEM_VCC)
    };
    _sensorTableLen = getSensorTableSize(_sensorTable);   
}