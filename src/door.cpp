#include "door.h"

Door::Door(int open_sensor_pin, int closed_sensor_pin, int relay_pin, void (*isrHandler)()) {
    this->open_sensor_pin = open_sensor_pin;
    this->closed_sensor_pin = closed_sensor_pin;
    this->relay_pin = relay_pin;
    open_sensor = new DoorSensor(this->open_sensor_pin);
    closed_sensor = new DoorSensor(this->closed_sensor_pin);
    door_relay = new Relay(this->relay_pin);

    open_sensor->configureInterrupt(isrHandler);
    closed_sensor->configureInterrupt(isrHandler);
}

Door::doorState Door::getState() {
    if (open_sensor->getState() == DoorSensor::SENSOR_STATE_OPEN && closed_sensor->getState() == DoorSensor::SENSOR_STATE_CLOSED) {
        this->state = DOOR_STATE_CLOSED;
    } else if (open_sensor->getState() == DoorSensor::SENSOR_STATE_CLOSED && closed_sensor->getState() == DoorSensor::SENSOR_STATE_OPEN) {
        this->state = DOOR_STATE_OPEN;
    } else if (open_sensor->getState() == DoorSensor::SENSOR_STATE_OPEN && closed_sensor->getState() == DoorSensor::SENSOR_STATE_OPEN) {
        this->state = DOOR_STATE_TRANSITIONING;
    } else {
        this->state = DOOR_STATE_ERROR;
    }
    return this->state;
}

const char* Door::getStateString() {
    Door::doorState cur_state = this->getState();
    if (cur_state == DOOR_STATE_CLOSED) {
        return "Closed";
    } else if (cur_state == DOOR_STATE_OPEN) {
        return "Open";
    } else if (cur_state == DOOR_STATE_TRANSITIONING) {
        return "Transitioning";
    } else if (cur_state == DOOR_STATE_ERROR) {
        return "Error";
    } else {
        return "Unknown";
    }
}

void Door::doorOpen() {
    if (this->getState() == DOOR_STATE_CLOSED) {
        door_relay->toggle();
    }
}

void Door::doorClose() {
    if (this->getState() == DOOR_STATE_OPEN) {
        door_relay->toggle();
    }
}
