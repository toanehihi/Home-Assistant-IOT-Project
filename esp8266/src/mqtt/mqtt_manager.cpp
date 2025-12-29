#include "mqtt_manager.h"
#include "../config/config.h"
#include "../config/mqtt_topics.h"
#include "../actuators/actuator_manager.h"

// External variables
// extern bool ledAutoMode;

/**
 * Cấu hình MQTT client
 */
void setupMQTT() {
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(handleMQTTMessage);
  mqttClient.setBufferSize(MQTT_BUFFER_SIZE);
  Serial.println("✓ MQTT client configured");
}

/**
 * Kết nối lại MQTT broker nếu mất kết nối
 */
void reconnectMQTT() {
  int attempts = 0;
  bool isFirstConnection = (mqttClientId == String(MQTT_CLIENT_ID_PREFIX_STR));
  
  while (!mqttClient.connected() && attempts < MQTT_RECONNECT_ATTEMPTS) {
    Serial.print("📡 Attempting MQTT connection... ");
    
    // Tạo client ID unique
    mqttClientId = String(MQTT_CLIENT_ID_PREFIX_STR) + String(random(0xffff), HEX);
    
    if (mqttClient.connect(mqttClientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("✓ Connected!");
      
      subscribeMQTTTopics();
      publishMQTTStatus();
      
      // Chỉ publish discovery khi kết nối lần đầu hoặc sau khi mất kết nối lâu
      if (isFirstConnection || attempts == 0) {
        publishMQTTDiscovery();
      }
      
    } else {
      Serial.print("✗ Failed (rc=");
      Serial.print(mqttClient.state());
      Serial.println(") | Retrying in 5 seconds...");
      delay(5000);
      attempts++;
    }
  }
  
  if (!mqttClient.connected()) {
    Serial.println("✗ MQTT connection failed after maximum attempts!");
  }
}

/**
 * Đăng ký subscribe các MQTT topics
 */
void subscribeMQTTTopics() {
  mqttClient.subscribe(MQTTTopics::LED_COMMAND);
  mqttClient.subscribe(MQTTTopics::BUZZER_COMMAND);
  Serial.println("✓ Subscribed to control topics");
}

/**
 * Publish trạng thái online của hệ thống
 */
void publishMQTTStatus() {
  mqttClient.publish(MQTTTopics::STATUS, "online", true);
}

/**
 * Tạo JSON string cho device info (dùng chung cho tất cả entities)
 */
String buildDeviceInfo() {
  String deviceInfo = "\"device\":{";
  deviceInfo += "\"identifiers\":[\"" + String(DEVICE_ID) + "\"],";
  deviceInfo += "\"name\":\"" + String(DEVICE_NAME) + "\",";
  deviceInfo += "\"manufacturer\":\"" + String(DEVICE_MANUFACTURER) + "\",";
  deviceInfo += "\"model\":\"" + String(DEVICE_MODEL) + "\",";
  deviceInfo += "\"sw_version\":\"" + String(DEVICE_SW_VERSION) + "\"";
  deviceInfo += "}";
  return deviceInfo;
}

/**
 * Publish Discovery config cho Temperature sensor
 */
void publishTemperatureDiscovery() {
  String config = "{";
  config += "\"name\":\"Temperature\",";
  config += "\"unique_id\":\"" + String(DEVICE_ID) + "_temperature\",";
  config += "\"state_topic\":\"" + String(MQTTTopics::TEMPERATURE_STATE) + "\",";
  config += "\"unit_of_measurement\":\"°C\",";
  config += "\"device_class\":\"temperature\",";
  config += "\"state_class\":\"measurement\",";
  config += "\"expire_after\":300,";
  config += buildDeviceInfo();
  config += "}";
  
  mqttClient.publish(MQTTTopics::DISCOVERY_TEMP_CONFIG, config.c_str(), true);
  Serial.println("  ✓ Temperature sensor discovery published");
}

/**
 * Publish Discovery config cho Humidity sensor
 */
void publishHumidityDiscovery() {
  String config = "{";
  config += "\"name\":\"Humidity\",";
  config += "\"unique_id\":\"" + String(DEVICE_ID) + "_humidity\",";
  config += "\"state_topic\":\"" + String(MQTTTopics::HUMIDITY_STATE) + "\",";
  config += "\"unit_of_measurement\":\"%\",";
  config += "\"device_class\":\"humidity\",";
  config += "\"state_class\":\"measurement\",";
  config += "\"expire_after\":300,";
  config += buildDeviceInfo();
  config += "}";
  
  mqttClient.publish(MQTTTopics::DISCOVERY_HUMIDITY_CONFIG, config.c_str(), true);
  Serial.println("  ✓ Humidity sensor discovery published");
}

/**
 * Publish Discovery config cho Motion binary sensor
 */
void publishMotionDiscovery() {
  String config = "{";
  config += "\"name\":\"Motion\",";
  config += "\"unique_id\":\"" + String(DEVICE_ID) + "_motion\",";
  config += "\"state_topic\":\"" + String(MQTTTopics::MOTION_STATE) + "\",";
  config += "\"device_class\":\"motion\",";
  config += "\"payload_on\":\"ON\",";
  config += "\"payload_off\":\"OFF\",";
  config += "\"off_delay\":60,";
  config += buildDeviceInfo();
  config += "}";
  
  mqttClient.publish(MQTTTopics::DISCOVERY_MOTION_CONFIG, config.c_str(), true);
  Serial.println("  ✓ Motion sensor discovery published");
}

/**
 * Publish Discovery config cho LED switch
 */
void publishLEDDiscovery() {
  String config = "{";
  config += "\"name\":\"LED Light\",";
  config += "\"unique_id\":\"" + String(DEVICE_ID) + "_led\",";
  config += "\"command_topic\":\"" + String(MQTTTopics::LED_COMMAND) + "\",";
  config += "\"state_topic\":\"" + String(MQTTTopics::LED_STATE) + "\",";
  config += "\"payload_on\":\"ON\",";
  config += "\"payload_off\":\"OFF\",";
  config += "\"state_on\":\"ON\",";
  config += "\"state_off\":\"OFF\",";
  config += "\"optimistic\":false,";
  config += "\"retain\":true,";
  config += "\"icon\":\"mdi:lightbulb\",";
  config += buildDeviceInfo();
  config += "}";
  
  mqttClient.publish(MQTTTopics::DISCOVERY_LED_CONFIG, config.c_str(), true);
  Serial.println("  ✓ LED switch discovery published");
}

/**
 * Publish Discovery config cho Buzzer switch
 */
void publishBuzzerDiscovery() {
  String config = "{";
  config += "\"name\":\"Buzzer\",";
  config += "\"unique_id\":\"" + String(DEVICE_ID) + "_buzzer\",";
  config += "\"command_topic\":\"" + String(MQTTTopics::BUZZER_COMMAND) + "\",";
  config += "\"state_topic\":\"" + String(MQTTTopics::BUZZER_STATE) + "\",";
  config += "\"payload_on\":\"ON\",";
  config += "\"payload_off\":\"OFF\",";
  config += "\"state_on\":\"ON\",";
  config += "\"state_off\":\"OFF\",";
  config += "\"optimistic\":false,";
  config += "\"retain\":true,";
  config += "\"icon\":\"mdi:bell\",";
  config += buildDeviceInfo();
  config += "}";
  
  mqttClient.publish(MQTTTopics::DISCOVERY_BUZZER_CONFIG, config.c_str(), true);
  Serial.println("  ✓ Buzzer switch discovery published");
}

/**
 * Publish tất cả MQTT Discovery configurations
 * 
 * MQTT Discovery cho phép Home Assistant tự động phát hiện và cấu hình
 * các thiết bị mà không cần khai báo thủ công trong configuration.yaml
 * 
 * Format: homeassistant/<component>/<node_id>/<object_id>/config
 * Home Assistant sẽ tự động đọc các config này và tạo entities tương ứng
 */
void publishMQTTDiscovery() {
  Serial.println();
  Serial.println("🔍 Publishing MQTT Discovery configurations...");
  
  publishTemperatureDiscovery();
  delay(100);
  
  publishHumidityDiscovery();
  delay(100);
  
  publishMotionDiscovery();
  delay(100);
  
  publishLEDDiscovery();
  delay(100);
  
  publishBuzzerDiscovery();
  delay(100);
  
  Serial.println("✓ MQTT Discovery completed!");
  Serial.println();
}

/**
 * Xử lý message nhận được từ MQTT broker
 */
void handleMQTTMessage(char* topic, byte* payload, unsigned int length) {
  // Chuyển đổi payload thành String
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("📩 MQTT Message [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);
  
  // Xử lý theo topic
  String topicStr = String(topic);
  
  if (topicStr == MQTTTopics::LED_COMMAND) {
    handleLEDControl(message);
  } 
  else if (topicStr == MQTTTopics::BUZZER_COMMAND) {
    handleBuzzerControl(message);
  }
}

/**
 * Xử lý điều khiển LED
 */
void handleLEDControl(String message) {
  message.toUpperCase();
  
  if (message == "ON") {
    turnOnLED();
  } 
  else if (message == "OFF") {
    turnOffLED();
  } 
}

/**
 * Xử lý điều khiển Buzzer
 */
void handleBuzzerControl(String message) {
  message.toUpperCase();
  
  if (message == "ON") {
    turnOnBuzzer();
  } 
  else {
    turnOffBuzzer();
  }
}

