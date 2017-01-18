#include "doorsensor.h"

DoorSensor::DoorSensor(int gpio_pin) {
    this->gpio_pin = gpio_pin;
    pinMode(this->gpio_pin, INPUT_PULLUP);
}

DoorSensor::sensorState DoorSensor::getState() {
    int current_state = digitalRead(this->gpio_pin);
    switch (current_state) {
        case HIGH:
            this->state = SENSOR_STATE_OPEN;
            break;
        case LOW:
            this->state = SENSOR_STATE_CLOSED;
            break;
        default:
            this->state = SENSOR_STATE_ERROR;
    }
    return this->state;
}

void DoorSensor::configureInterrupt(void (*isrHandler)()) {
    attachInterrupt(this->gpio_pin, isrHandler, CHANGE);
}
