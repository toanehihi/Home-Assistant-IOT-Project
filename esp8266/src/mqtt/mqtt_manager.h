#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

// Forward declarations
extern WiFiClient espClient;
extern PubSubClient mqttClient;
extern String mqttClientId;

void setupMQTT();

void reconnectMQTT();

void subscribeMQTTTopics();

void publishMQTTStatus();

void publishMQTTDiscovery();

void handleMQTTMessage(char* topic, byte* payload, unsigned int length);

void handleLEDControl(String message);

void handleBuzzerControl(String message);

#endif // MQTT_MANAGER_H

