#include "actuator_manager.h"
#include "../config/config.h"
#include "../config/mqtt_topics.h"
#include <PubSubClient.h>

// External variables
extern PubSubClient mqttClient;
extern bool ledState;
extern bool buzzerState;

void turnOnLED() {
  digitalWrite(PIN_RELAY_LED, RELAY_ON);
  ledState = true;
  mqttClient.publish(MQTTTopics::LED_STATE, "ON", true);
  Serial.println("💡 LED: ON");
}

void turnOffLED() {
  digitalWrite(PIN_RELAY_LED, RELAY_OFF);
  ledState = false;
  mqttClient.publish(MQTTTopics::LED_STATE, "OFF", true);
  Serial.println("💡 LED: OFF");
}

void turnOnBuzzer() {
  digitalWrite(PIN_RELAY_BUZZER, RELAY_ON);
  buzzerState = true;
  mqttClient.publish(MQTTTopics::BUZZER_STATE, "ON", true);
  Serial.println("🔊 Buzzer: ON");
}

void turnOffBuzzer() {
  digitalWrite(PIN_RELAY_BUZZER, RELAY_OFF);
  buzzerState = false;
  mqttClient.publish(MQTTTopics::BUZZER_STATE, "OFF", true);
  Serial.println("🔊 Buzzer: OFF");
}

