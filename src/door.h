#ifndef DOOR_H
#define DOOR_H

#include "global.h"
#include "doorsensor.h"
#include "relay.h"


class Door {
    enum doorState {
        DOOR_STATE_OPEN,
        DOOR_STATE_CLOSED,
        DOOR_STATE_TRANSITIONING,
        DOOR_STATE_ERROR
    };
public:
    Door(int open_sensor_pin, int closed_sensor_pin, int relay_pin, void (*isrHandler)());
    void doorOpen();
    void doorClose();
    doorState getState();
    const char* getStateString();
private:
    int open_sensor_pin;
    int closed_sensor_pin;
    int relay_pin;
    doorState state;
    DoorSensor* open_sensor;
    DoorSensor* closed_sensor;
    Relay* door_relay;
protected:
};

#endif
