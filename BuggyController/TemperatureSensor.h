#ifndef TemperatureSensor_h
#define TemperatureSensor_h 

typedef struct tempNode {
    int input;
    double temperature;
} tempNode;

tempNode constructTempNode(int input, double temperature);

typedef enum {T_COLD, T_NORMAL, T_REGULATED, T_HOT} tempStatusType;

typedef struct tempStatusNode {
    double temperature;
    tempStatusType tempStatus;
} tempStatusNode;

tempStatusNode constructTempStatusNode(double temperature, tempStatusType tempStatus);

class TemperatureSensor {
public:
    static double analog2temp(int rawValue, int sensorType);
    static tempStatusType getTempStatus(double temperature, double minTemp, double regTemp, double maxTemp, bool ignoreTemps);
};


#endif