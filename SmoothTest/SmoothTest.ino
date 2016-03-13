#include "arduino.h"
#include <DigitalSmooth.h>

#define THROTTLE_PIN A3
#define SAMPLE_SIZE 25
#define BAUDRATE 115200
#define DEBUG_PRINT_DELAY 200
#define BUFFSIZ 150

DigitalSmooth* smoother;
int rawValue;
int smoothedValue;

void setup()
{
    Serial.begin(BAUDRATE);
    pinMode(THROTTLE_PIN, INPUT);
    smoother = new DigitalSmooth(SAMPLE_SIZE, 0.15);
}

void loop()
{
    rawValue = analogRead(THROTTLE_PIN);
    smoothedValue = smoother->smoothData(rawValue);
    char buffer[BUFFSIZ];
    snprintDebugInfo(buffer, BUFFSIZ);
    Serial.println(buffer);

    delay(DEBUG_PRINT_DELAY);
}

int snprintDebugInfo(char* buffer, int charsRemaining){
    int charsPrinted = 0;

    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "RAW:%3d|", rawValue );
    charsPrinted += snprintf((buffer+charsPrinted), abs(charsRemaining-charsPrinted), "SMT:%3d|", smoothedValue );
    smoother->snprintSamples((buffer+charsPrinted), abs(charsRemaining-charsPrinted));

    return charsPrinted;
}
