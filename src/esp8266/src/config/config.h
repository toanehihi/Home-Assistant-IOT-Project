#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// WIFI CONFIGURATION
// ============================================================================
const char* WIFI_SSID = "J19";
const char* WIFI_PASSWORD = "hoangchimbe";
const int WIFI_CONNECT_TIMEOUT = 30;  // Số lần thử kết nối WiFi (30 * 500ms = 15s)

// ============================================================================
// MQTT CONFIGURATION
// ============================================================================
const char* MQTT_SERVER = "192.168.1.10";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASS = "";
const int MQTT_RECONNECT_ATTEMPTS = 5;
const int MQTT_BUFFER_SIZE = 1024;  // Tăng buffer cho discovery JSON
const String MQTT_CLIENT_ID_PREFIX = "ESP8266-SmartHome-";

// ============================================================================
// MQTT DISCOVERY CONFIGURATION
// ============================================================================
const char* DISCOVERY_PREFIX = "homeassistant";
const char* DEVICE_NAME = "Smart Home ESP8266";
const char* DEVICE_ID = "esp8266_smarthome";
const char* DEVICE_MANUFACTURER = "Smart Home Project";
const char* DEVICE_MODEL = "ESP8266 + DHT11 + SR501";
const char* DEVICE_SW_VERSION = "1.0.0";

// ============================================================================
// GPIO PIN DEFINITIONS
// ============================================================================
#define PIN_DHT11_DATA    14    // D5 = GPIO14 - DHT11 Data pin
#define PIN_SR501_OUT     2     // D4 = GPIO2 - SR501 Motion sensor output
#define PIN_RELAY_LED     5     // D1 = GPIO5 - Relay IN1 (LED control)
#define PIN_RELAY_BUZZER  4     // D2 = GPIO4 - Relay IN2 (Buzzer control)

// ============================================================================
// SENSOR CONFIGURATION
// ============================================================================
#define DHT_TYPE          DHT11
#define SENSOR_READ_INTERVAL  3000    // Đọc cảm biến mỗi 3 giây (ms)

// ============================================================================
// THRESHOLDS & TIMEOUTS
// ============================================================================
#define TEMP_THRESHOLD_HIGH    36.0   // Ngưỡng nhiệt độ cảnh báo (°C)
#define TEMP_CHANGE_THRESHOLD  0.5    // Ngưỡng thay đổi nhiệt độ để publish (°C)
#define HUMIDITY_CHANGE_THRESHOLD 2.0  // Ngưỡng thay đổi độ ẩm để publish (%)
#define MOTION_LED_TIMEOUT     120000  // Thời gian đèn tự tắt sau khi phát hiện chuyển động (ms = 2 phút)

// ============================================================================
// RELAY LOGIC
// ============================================================================
#define RELAY_ON   LOW
#define RELAY_OFF  HIGH

// ============================================================================
// SERIAL CONFIGURATION
// ============================================================================
#define SERIAL_BAUD_RATE  115200

#endif // CONFIG_H

