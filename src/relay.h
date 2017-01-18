#ifndef RELAY_H
#define RELAY_H

#include "global.h"
#include <Arduino.h>

#define HOLDTIME 500

class Relay {
public:
    Relay(int gpio_pin);
    void toggle();
private:
    int gpio_pin;
protected:
};

#endif
