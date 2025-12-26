#ifndef MQTT_TOPICS_H
#define MQTT_TOPICS_H

namespace MQTTTopics {
  // Status
  const char* STATUS = "homeassistant/status";
  
  // Discovery Topics (for Home Assistant auto-discovery)
  const char* DISCOVERY_TEMP_CONFIG = "homeassistant/sensor/esp8266_smarthome/temperature/config";
  const char* DISCOVERY_HUMIDITY_CONFIG = "homeassistant/sensor/esp8266_smarthome/humidity/config";
  const char* DISCOVERY_MOTION_CONFIG = "homeassistant/binary_sensor/esp8266_smarthome/motion/config";
  const char* DISCOVERY_LED_CONFIG = "homeassistant/switch/esp8266_smarthome/led/config";
  const char* DISCOVERY_BUZZER_CONFIG = "homeassistant/switch/esp8266_smarthome/buzzer/config";
  
  // State Topics (used by discovery)
  const char* TEMPERATURE_STATE = "homeassistant/sensor/esp8266_smarthome/temperature/state";
  const char* HUMIDITY_STATE = "homeassistant/sensor/esp8266_smarthome/humidity/state";
  const char* MOTION_STATE = "homeassistant/binary_sensor/esp8266_smarthome/motion/state";
  
  // Command Topics (used by discovery)
  const char* LED_COMMAND = "homeassistant/switch/esp8266_smarthome/led/set";
  const char* BUZZER_COMMAND = "homeassistant/switch/esp8266_smarthome/buzzer/set";
  
  // State Topics (used by discovery)
  const char* LED_STATE = "homeassistant/switch/esp8266_smarthome/led/state";
  const char* BUZZER_STATE = "homeassistant/switch/esp8266_smarthome/buzzer/state";
  
  // Alerts
  const char* TEMP_ALERT = "homeassistant/alert/temperature";
}

#endif // MQTT_TOPICS_H

