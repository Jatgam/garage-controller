#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "global.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// #define DS18B20_ID 0x28

// Temp Sensor Precision in bits. 9, 10, 11, or 12
#define TEMP_PRECISION 9

class Temperature {
public:
    Temperature();
    float getCelsius();
    float getFahrenheit();
    char* getCelsiusChar();
    char* getFahrenheitChar();
private:
    OneWire* oneWire;
    DallasTemperature* temperatureSensors;
    DeviceAddress DS18B20[3];
    int16_t raw;
};

#endif
