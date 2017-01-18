#include "relay.h"

Relay::Relay(int gpio_pin) {
    this->gpio_pin = gpio_pin;
    pinMode(this->gpio_pin, OUTPUT);
    digitalWrite(this->gpio_pin,HIGH);
}

void Relay::toggle() {
    digitalWrite(this->gpio_pin,LOW);
    delay(HOLDTIME);
    digitalWrite(this->gpio_pin,HIGH);
}
