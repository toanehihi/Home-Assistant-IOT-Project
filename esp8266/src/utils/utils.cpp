#include "utils.h"
#include "../config/config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Forward declarations
extern PubSubClient mqttClient;
extern bool ledState;
extern bool buzzerState;
extern bool ledAutoMode;
extern int motionState;
extern float lastTemperature;
extern float lastHumidity;

/**
 * Khởi tạo các chân GPIO
 */
void initializeGPIO() {
  // Cấu hình chân output (Relay)
  pinMode(PIN_RELAY_LED, OUTPUT);
  pinMode(PIN_RELAY_BUZZER, OUTPUT);
  
  // Cấu hình chân input (Sensor)
  pinMode(PIN_SR501_OUT, INPUT);
  
  // Tắt tất cả relay ban đầu
  digitalWrite(PIN_RELAY_LED, RELAY_OFF);
  digitalWrite(PIN_RELAY_BUZZER, RELAY_OFF);
  
  Serial.println("✓ GPIO pins initialized");
}

/**
 * Test các relay (nháy 2 lần)
 */
void testRelays() {
  Serial.println("🔧 Testing relays...");
  
  for (int i = 0; i < 2; i++) {
    digitalWrite(PIN_RELAY_LED, RELAY_ON);
    digitalWrite(PIN_RELAY_BUZZER, RELAY_ON);
    delay(200);
    
    digitalWrite(PIN_RELAY_LED, RELAY_OFF);
    digitalWrite(PIN_RELAY_BUZZER, RELAY_OFF);
    delay(200);
  }
  
  Serial.println("✓ Relay test completed");
}

/**
 * In trạng thái hệ thống ra Serial
 */
void printSystemStatus() {
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("         SYSTEM STATUS");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.print("WiFi:    ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "✓ Connected" : "✗ Disconnected");
  Serial.print("MQTT:    ");
  Serial.println(mqttClient.connected() ? "✓ Connected" : "✗ Disconnected");
  Serial.print("LED:     ");
  Serial.print(ledState ? "ON" : "OFF");
  Serial.print(ledAutoMode ? " (AUTO)" : " (MANUAL)");
  Serial.println();
  Serial.print("Buzzer:  ");
  Serial.println(buzzerState ? "ON" : "OFF");
  Serial.print("Motion:  ");
  Serial.println(motionState == HIGH ? "Detected" : "None");
  Serial.print("Temp:    ");
  Serial.print(lastTemperature);
  Serial.print("°C | Hum: ");
  Serial.print(lastHumidity);
  Serial.println("%");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
}

