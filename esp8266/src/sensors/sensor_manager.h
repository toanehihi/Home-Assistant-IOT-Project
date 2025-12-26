#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <DHT.h>
#include <PubSubClient.h>

// Forward declarations
extern DHT dht;
extern PubSubClient mqttClient;
extern int motionState;
extern int lastMotionState;
extern unsigned long motionDetectedTime;
extern bool ledAutoMode;
extern bool tempAlertActive;
extern float lastTemperature;
extern float lastHumidity;

void readDHT11Sensor();

void readMotionSensor();

void handleTemperatureAlert(float temperature);

void handleMotionAutoLED();

#endif // SENSOR_MANAGER_H

