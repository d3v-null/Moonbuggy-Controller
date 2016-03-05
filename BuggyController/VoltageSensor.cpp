#include "VoltageSensor.h"

sensorNode constructSensorNode(int input, double sensorVal){
    sensorNode node;
    node.input = input;
    node.sensorVal = sensorVal;
    return node;
}

int snprintSensorNode(char* buffer, int charsRemaining, sensorNode node){
    return snprintf(buffer, charsRemaining, "{%4d:%4d}", node.input, (int)(node.sensorVal * 100));
}

// int getSensorTableSize(sensorNode* sensorTable){
//     return sizeof(sensorTable) / sizeof(sensorTable[0]);
// }

VoltageSensor::VoltageSensor() {
    // Serial.println("VoltageSensor Constructor"); delay(100);
    _pinsSet = false;
    _pinsInit = false;
    _rawVal = 0;
    _sensorTable = new sensorNode[2];
    int i=0;
    _sensorTable[i++] = constructSensorNode(0,                      0.0);
    _sensorTable[i++] = constructSensorNode(SYSTEM_ANALOGUE_MAX,    SYSTEM_VCC);
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

int VoltageSensor::snprintSensorTable(char* buffer, int charsRemaining){
    char*start = buffer;
    int charsPrinted = 0;
    int i;
    for(i=0; i < SIZEOFTABLE(_sensorTable) ; i++){
        if(charsPrinted < charsRemaining){
            charsPrinted += snprintSensorNode(buffer, charsRemaining - charsPrinted, _sensorTable[i]);
        } else{
            break;
        }
    }
    return charsPrinted;
}

int VoltageSensor::getSensorTableSize(){
    return SIZEOFTABLE(_sensorTable) ;
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
    for(i=1; i < SIZEOFTABLE(_sensorTable) ; i++){
        if (_sensorTable[i].input > rawVal){
            sensorVal = _sensorTable[i-1].sensorVal + 
                (rawVal - _sensorTable[i].input) * 
                (double)(_sensorTable[i].sensorVal - _sensorTable[i-1].sensorVal)/
                (double)(_sensorTable[i].input - _sensorTable[i-1].input);
            break;
        }
    }
    return sensorVal;
}

NormalizedVoltageSensor::NormalizedVoltageSensor() {
    // Serial.println("NormalizedVoltageSensor Constructor"); delay(100);
    setInputConstraints();
}

int NormalizedVoltageSensor::getRawVal(){
    return constrain(
        _rawVal, 
        _sensorMin, 
        _sensorMax
    );
}

void NormalizedVoltageSensor::setInputConstraints(int minimum, int maximum) {
    if(minimum >= 0 and maximum > minimum and maximum <= SYSTEM_ANALOGUE_MAX){
        _sensorMin = minimum;
        _sensorMax = maximum;
        delete[] _sensorTable;
        _sensorTable = new sensorNode[2];
        int i=0;
        _sensorTable[i++] = constructSensorNode(_sensorMin,    0.0);
        _sensorTable[i++] = constructSensorNode(_sensorMax,    1.0);
    }
}

// double NormalizedVoltageSensor::getSensorVal(){
//     return VoltageSensor::getSensorVal();
// }

VoltageDividerSensor::VoltageDividerSensor(){
    setSensorMultiplier();
}

void VoltageDividerSensor::setSensorMultiplier(double sensorMultiplier){
    if(sensorMultiplier > 0.0){
        delete[] _sensorTable;
        _sensorTable = new sensorNode[2];
        int i=0;
        _sensorTable[i++] = constructSensorNode(0,                      0.0);
        _sensorTable[i++] = constructSensorNode(SYSTEM_ANALOGUE_MAX,    sensorMultiplier * SYSTEM_VCC); 
    }
}