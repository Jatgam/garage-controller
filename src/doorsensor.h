#ifndef DOORSENSOR_H
#define DOORSENSOR_H

#include "global.h"
#include <Arduino.h>

class DoorSensor {

public:
    enum sensorState {
        SENSOR_STATE_OPEN,
        SENSOR_STATE_CLOSED,
        SENSOR_STATE_ERROR
    };
    DoorSensor(int gpio_pin);
    sensorState getState();
    void configureInterrupt(void (*isrHandler)());
private:
    int gpio_pin;
    sensorState state;
protected:
};

#endif
