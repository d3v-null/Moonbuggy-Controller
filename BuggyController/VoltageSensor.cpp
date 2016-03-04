#include "VoltageSensor.h"

#define SIZEOFTABLE( TABLE ) sizeof( TABLE ) / sizeof( TABLE[0] );

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
    _pinsSet = false;
    _pinsInit = false;
    _sensorTable = {
        constructSensorNode(0,                      0.0),
        constructSensorNode(SYSTEM_ANALOGUE_MAX,    SYSTEM_VCC)
    };
    // _sensorTableLen = SIZEOFTABLE(_sensorTable);
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
    for(i=1; i < SIZEOFTABLE(_sensorTable) ; i++){
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
    setInputConstraints();
}

void NormalizedVoltageSensor::setInputConstraints(int minimum, int maximum) {
    if(minimum >= 0 and maximum <= SYSTEM_ANALOGUE_MAX){
        _sensorMin = minimum;
        _sensorMax = maximum;
        _sensorTable = {
            constructSensorNode(_sensorMin,    0.0),
            constructSensorNode(_sensorMax,    1.0)
        };
        // _sensorTableLen = SIZEOFTABLE(_sensorTable); 
    }
}

double NormalizedVoltageSensor::getSensorVal(){
    _rawVal = constrain(
        _rawVal, 
        _sensorMin, 
        _sensorMax
    );
    return VoltageSensor::getSensorVal();
}

VoltageDividerSensor::VoltageDividerSensor(){
    setSensorMultiplier();
}

void VoltageDividerSensor::setSensorMultiplier(double sensorMultiplier){
    _sensorTable = {
        constructSensorNode(0,                      0.0),
        constructSensorNode(SYSTEM_ANALOGUE_MAX,    sensorMultiplier * SYSTEM_VCC)
    };
    // _sensorTableLen = SIZEOFTABLE(_sensorTable);   
}