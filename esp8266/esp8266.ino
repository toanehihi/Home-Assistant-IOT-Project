#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Include các module từ thư mục src/
#include "src/config/config.h"
#include "src/config/mqtt_topics.h"
#include "src/wifi/wifi_manager.h"
#include "src/mqtt/mqtt_manager.h"
#include "src/sensors/sensor_manager.h"
#include "src/actuators/actuator_manager.h"
#include "src/utils/utils.h"

// ============================================================================
// GLOBAL OBJECTS - ĐỐI TƯỢNG TOÀN CỤC
// ============================================================================

DHT dht(PIN_DHT11_DATA, DHT_TYPE);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ============================================================================
// STATE VARIABLES - BIẾN TRẠNG THÁI
// ============================================================================

// Timing
unsigned long lastSensorRead = 0;

// Motion Sensor State
int motionState = LOW;
int lastMotionState = LOW;
unsigned long motionDetectedTime = 0;

// Device States
bool ledState = false;
bool buzzerState = false;
// bool ledAutoMode = true;        // Chế độ tự động bật đèn khi có chuyển động
bool tempAlertActive = false;   // Cảnh báo nhiệt độ đang hoạt động

// Sensor Values
float lastTemperature = 0.0;
float lastHumidity = 0.0;

// MQTT Client ID
String mqttClientId = MQTT_CLIENT_ID_PREFIX_STR;

// ============================================================================
// ARDUINO MAIN FUNCTIONS - HÀM CHÍNH ARDUINO
// ============================================================================

/**
 * Hàm setup - Chạy một lần khi khởi động
 */
void setup() {
  // Khởi tạo Serial
  Serial.begin(SERIAL_BAUD_RATE);
  delay(100);
  
  // Header
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║   SMART HOME - HOME ASSISTANT MQTT     ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Khởi tạo GPIO
  initializeGPIO();
  
  // Khởi động DHT11
  dht.begin();
  Serial.println("✓ DHT11 sensor initialized");
  
  // Kết nối WiFi
  setupWiFi();
  
  // Cấu hình MQTT
  setupMQTT();
  
  Serial.println();
  Serial.println("✓ Setup completed successfully!");
  Serial.println();
  
  // Test relay - Disabled để tránh nhiễu nguồn khi SR501 active
  // testRelays();
  Serial.println();
}

/**
 * Hàm loop - Chạy liên tục
 */
void loop() {
  // Kiểm tra và kết nối lại WiFi nếu mất kết nối
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("✗ WiFi disconnected! Reconnecting...");
    setupWiFi();
  }
  
  // Kiểm tra và kết nối lại MQTT nếu mất kết nối
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  // Xử lý MQTT messages
  mqttClient.loop();
  
  // Đọc cảm biến theo chu kỳ
  unsigned long currentTime = millis();
  if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = currentTime;
    readDHT11Sensor();
  }
  
  // Đọc cảm biến chuyển động (real-time)
  readMotionSensor();
  
  // Giữ kết nối ổn định
  yield();
}
