#include "temperature.h"

Temperature::Temperature() {
    oneWire = new OneWire(TEMP_SENSOR_PIN);
    temperatureSensors = new DallasTemperature(oneWire);

    temperatureSensors->begin();
    int available = temperatureSensors->getDeviceCount();
    for(int x = 0; x!= available; x++) {
        if(temperatureSensors->getAddress(DS18B20[x], x)) {
            temperatureSensors->setResolution(DS18B20[x], TEMP_PRECISION);
        }
    }
    temperatureSensors->setWaitForConversion(true);
}

float Temperature::getCelsius() {
    int address = 0; //Use the first sensor and ignore rest for now
    if (temperatureSensors->getAddress(DS18B20[address], address)) {
        temperatureSensors->requestTemperatures();
        return temperatureSensors->getTempC(DS18B20[address]);
    }
    else
        return 0;
}

float Temperature::getFahrenheit() {
    int address = 0; //Use the first sensor and ignore rest for now
    if (temperatureSensors->getAddress(DS18B20[address], address)) {
        temperatureSensors->requestTemperatures();
        return temperatureSensors->getTempF(DS18B20[address]);
    }
    else
        return 0;
}

char* Temperature::getCelsiusChar() {
    char t[8];
    return dtostrf(this->getCelsius(), 7, 2, t);
}

char* Temperature::getFahrenheitChar() {
    char t[8];
    return dtostrf(this->getFahrenheit(), 7, 2, t);
}
