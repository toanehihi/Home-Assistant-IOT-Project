#include "sensor_manager.h"
#include "../config/config.h"
#include "../config/mqtt_topics.h"
#include "../actuators/actuator_manager.h"
#include <DHT.h>
#include <PubSubClient.h>

// External variables
extern DHT dht;
extern PubSubClient mqttClient;
extern int motionState;
extern int lastMotionState;
extern unsigned long motionDetectedTime;
extern bool ledAutoMode;
extern bool tempAlertActive;
extern float lastTemperature;
extern float lastHumidity;

/**
 * Đọc cảm biến DHT11 (nhiệt độ & độ ẩm)
 */
void readDHT11Sensor() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Kiểm tra lỗi đọc
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("✗ Failed to read from DHT11 sensor!");
    return;
  }
  
  // Chỉ publish nếu giá trị thay đổi đáng kể (tránh spam MQTT)
  bool tempChanged = abs(temperature - lastTemperature) > TEMP_CHANGE_THRESHOLD;
  bool humChanged = abs(humidity - lastHumidity) > HUMIDITY_CHANGE_THRESHOLD;
  
  if (tempChanged || humChanged) {
    Serial.print("🌡️  Temperature: ");
    Serial.print(temperature);
    Serial.print("°C  |  💧 Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    
    // Publish lên MQTT
    mqttClient.publish(MQTTTopics::TEMPERATURE_STATE, String(temperature).c_str(), true);
    mqttClient.publish(MQTTTopics::HUMIDITY_STATE, String(humidity).c_str(), true);
    
    // Cập nhật giá trị đã lưu
    lastTemperature = temperature;
    lastHumidity = humidity;
  }
  
  // Xử lý cảnh báo nhiệt độ cao
  handleTemperatureAlert(temperature);
}

/**
 * Xử lý cảnh báo nhiệt độ cao
 */
void handleTemperatureAlert(float temperature) {
  if (temperature > TEMP_THRESHOLD_HIGH) {
    if (!tempAlertActive) {
      Serial.println("⚠️  HIGH TEMPERATURE ALERT!");
      Serial.print("  Current: ");
      Serial.print(temperature);
      Serial.print("°C | Threshold: ");
      Serial.print(TEMP_THRESHOLD_HIGH);
      Serial.println("°C");
      
      // Bật buzzer cảnh báo
      turnOnBuzzer();
      
      // Publish cảnh báo
      String alertMsg = "High temperature: " + String(temperature) + "°C";
      mqttClient.publish(MQTTTopics::TEMP_ALERT, alertMsg.c_str());
      
      tempAlertActive = true;
    }
  } 
  else {
    // Tắt cảnh báo khi nhiệt độ trở lại bình thường
    if (tempAlertActive) {
      Serial.println("✓ Temperature back to normal");
      turnOffBuzzer();
      tempAlertActive = false;
      mqttClient.publish(MQTTTopics::TEMP_ALERT, "Normal");
    }
  }
}

/**
 * Đọc cảm biến chuyển động SR501
 */
void readMotionSensor() {
  motionState = digitalRead(PIN_SR501_OUT);
  
  // Phát hiện chuyển động mới (rising edge)
  if (motionState == HIGH && lastMotionState == LOW) {
    Serial.println("🚶 Motion detected!");
    
    // Publish lên MQTT
    mqttClient.publish(MQTTTopics::MOTION_STATE, "ON", true);
    
    // Tự động bật đèn LED (chỉ khi ở chế độ AUTO)
    if (ledAutoMode) {
      turnOnLED();
      motionDetectedTime = millis();
      Serial.println("💡 LED turned ON automatically (motion detected)");
    }
    
    lastMotionState = HIGH;
  }
  
  // Kết thúc chuyển động (falling edge)
  if (motionState == LOW && lastMotionState == HIGH) {
    Serial.println("🚶 Motion ended");
    mqttClient.publish(MQTTTopics::MOTION_STATE, "OFF", true);
    lastMotionState = LOW;
  }
  
  // Xử lý tự động tắt đèn sau timeout
  handleMotionAutoLED();
}

/**
 * Xử lý tự động tắt đèn sau khi hết chuyển động
 */
void handleMotionAutoLED() {
  extern bool ledState;
  
  if (ledAutoMode && ledState && motionState == LOW) {
    unsigned long elapsed = millis() - motionDetectedTime;
    
    if (elapsed > MOTION_LED_TIMEOUT) {
      turnOffLED();
      Serial.println("💡 LED turned OFF automatically (timeout)");
    }
  }
}

