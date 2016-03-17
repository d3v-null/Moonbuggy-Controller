
#include "ThrottleSensor.h"



ThrottleSensor::ThrottleSensor(){
    setStatusBounds();
} 

void ThrottleSensor::setStatusBounds(double throttleZero, double throttleBoost){
    if(throttleZero >= 0.0 and throttleBoost > throttleZero and throttleBoost <= 1.0){
        double thresholds[] =       {throttleZero,  throttleBoost};
        throttleStatusType statuses[] = {TH_ZERO,       TH_NORMAL};
        for(int i=0; i<_statusNodes; i++){
            _statusTable[i].threshold = thresholds[i];
            _statusTable[i].statusVal = statuses[i];
        }
    }
}

throttleStatusType ThrottleSensor::getStatus(){
    throttleStatusType statusVal = TH_BOOST;
    double sensorVal = getSensorVal();
    int i;
    for( i=0; i < _statusNodes; i++) {
        if(sensorVal < _statusTable[i].threshold){
            statusVal = _statusTable[i].statusVal;
            break;
        } 
    }
    return statusVal;
}

void ThrottleSensor::initSmoother(){
    _smoother = new DigitalSmooth(10);
    _smootherInit = true;
}

// void ThrottleSensor::init(){
//     initSensorTable();
//     initSmoother();
// }

int ThrottleSensor::snprintStatusString(char* buffer, int charsRemaining, throttleStatusType statusVal){
    char* statusStr = "???";
    switch (statusVal) {
        case TH_ZERO:
          statusStr = "ZER";
          break;
        case TH_NORMAL:
          statusStr = "NOR";
          break;
        case TH_BOOST:
          statusStr = "BST";
          break;
        default:
          break;
    }
    return snprintf(buffer, charsRemaining, statusStr);
}

int ThrottleSensor::snprintStatusNode(char* buffer, int charsRemaining, throttleStatusNode node){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "{%4d:", (int)(100 * node.threshold) );
    charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), node.statusVal );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "}" );
    return charsPrinted;
}

int ThrottleSensor::snprintStatusTable(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "[");
    for(int i=0; i < _statusNodes; i++) {
        charsPrinted += snprintStatusNode((buffer+charsPrinted), abs(charsRemaining-charsPrinted), _statusTable[i]);
    }
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "]");
    return charsPrinted;
}

// int VoltageSensor::snprintValLabel(char* buffer, int charsRemaining){
//     return snprintf(buffer, charsRemaining, "VAL:");
// }

int ThrottleSensor::snprintReadings(char* buffer, int charsRemaining){
    int charsPrinted = 0;
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "PIN:%3d|",(int)(_sensorPin) );
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "SNT@%d:", (int)(_sensorTable)%1000);
    // charsPrinted += snprintSensorTable((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "STT:");
    // charsPrinted += snprintStatusTable((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "PER:%3d",(int)(100 * getSensorVal()) );
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "SMT:" );
    // charsPrinted += _smoother->snprintSamples((buffer+charsPrinted), abs(charsRemaining-charsPrinted));

    
    charsPrinted += VoltageSensor::snprintReadings((buffer+charsPrinted), abs(charsRemaining-charsPrinted));
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);


    // #ifndef DATA_LOGGING
    //     charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "VAL:" );
    // #endif
    // charsPrinted += strlen( dtostrf(getSensorVal(), FLOAT_WIDTH, FLOAT_PREC, (buffer+charsPrinted))) ;
    // charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    // #ifdef CALLIBRATE_SENSORS
    //     #ifndef DATA_LOGGING
    //         charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "RAW:");
    //     #endif
    //     charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_CELL_FMT_D,(getRawVal()));
    //     charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    // #endif
    #ifndef DATA_LOGGING
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "STS:");
        charsPrinted += snprintStatusString((buffer+charsPrinted), abs(charsRemaining-charsPrinted), getStatus());
        charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), DEBUG_DELIMETER);
    #endif
    return charsPrinted-1;
}