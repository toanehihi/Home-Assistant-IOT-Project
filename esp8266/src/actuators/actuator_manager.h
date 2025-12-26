#ifndef ACTUATOR_MANAGER_H
#define ACTUATOR_MANAGER_H

#include <PubSubClient.h>

// Forward declarations
extern PubSubClient mqttClient;
extern bool ledState;
extern bool buzzerState;

void turnOnLED();

void turnOffLED();

void turnOnBuzzer();

void turnOffBuzzer();

#endif // ACTUATOR_MANAGER_H

