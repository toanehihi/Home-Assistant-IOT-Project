# ESP8266 Smart Home - Home Assistant Integration

Hệ thống Smart Home sử dụng ESP8266 kết nối với Home Assistant qua MQTT, hỗ trợ tự động phát hiện thiết bị (MQTT Discovery) và điều khiển từ xa.

## 📋 Mục lục

- [Tính năng](#-tính-năng)
- [Phần cứng cần thiết](#-phần-cứng-cần-thiết)
- [Sơ đồ kết nối](#-sơ-đồ-kết-nối)
- [Cài đặt](#-cài-đặt)
- [Cấu hình](#-cấu-hình)
- [Cấu trúc dự án](#-cấu-trúc-dự-án)
- [MQTT Topics](#-mqtt-topics)
- [Home Assistant Integration](#-home-assistant-integration)
- [Sử dụng](#-sử-dụng)
- [Troubleshooting](#-troubleshooting)

## ✨ Tính năng

- **Cảm biến nhiệt độ & độ ẩm (DHT11)**: Đọc và gửi dữ liệu lên Home Assistant
- **Cảm biến chuyển động (SR501)**: Phát hiện chuyển động và tự động bật đèn
- **Điều khiển LED**: Bật/tắt đèn qua Home Assistant hoặc tự động khi có chuyển động
- **Điều khiển Buzzer**: Cảnh báo khi nhiệt độ cao
- **MQTT Discovery**: Tự động cấu hình trong Home Assistant, không cần khai báo YAML thủ công
- **Auto-reconnect**: Tự động kết nối lại WiFi và MQTT khi mất kết nối
- **Temperature Alert**: Cảnh báo và tự động bật buzzer khi nhiệt độ vượt ngưỡng

## 🔧 Phần cứng cần thiết

| Component | Mô tả | Số lượng |
|-----------|-------|----------|
| ESP8266 | NodeMCU hoặc Wemos D1 Mini | 1 |
| DHT11 | Cảm biến nhiệt độ & độ ẩm | 1 |
| SR501 | Cảm biến chuyển động PIR | 1 |
| Relay Module | Module relay 2 kênh | 1 |
| LED | Đèn LED (hoặc đèn 220V qua relay) | 1 |
| Buzzer | Còi báo động | 1 |
| Resistor | 10kΩ (cho DHT11) | 1 |
| Breadboard & Jumper wires | Để kết nối | - |

## 🔌 Sơ đồ kết nối

```
ESP8266          Component
--------         --------
GPIO14 (D5)  --> DHT11 Data
GPIO2  (D4)  --> SR501 OUT
GPIO5  (D1)  --> Relay IN1 (LED)
GPIO4  (D2)  --> Relay IN2 (Buzzer)
3.3V          --> DHT11 VCC, SR501 VCC, Relay VCC
GND           --> DHT11 GND, SR501 GND, Relay GND
                DHT11 Data --[10kΩ]--> 3.3V (pull-up)
```

### Chi tiết chân GPIO:

- **DHT11 Data**: GPIO14 (D5)
- **SR501 OUT**: GPIO2 (D4)
- **Relay IN1 (LED)**: GPIO5 (D1)
- **Relay IN2 (Buzzer)**: GPIO4 (D2)

## 📦 Cài đặt

### 1. Cài đặt Arduino IDE

1. Tải và cài đặt [Arduino IDE](https://www.arduino.cc/en/software)
2. Thêm ESP8266 Board Support:
   - File → Preferences → Additional Board Manager URLs
   - Thêm: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   - Tools → Board → Boards Manager → Tìm "esp8266" → Install

### 2. Cài đặt Libraries

Cài đặt các thư viện sau qua Library Manager (Sketch → Include Library → Manage Libraries):

- **ESP8266WiFi** (đã có sẵn với ESP8266 board)
- **PubSubClient** by Nick O'Leary
- **DHT sensor library** by Adafruit

### 3. Clone hoặc tải project

```bash
git clone <repository-url>
cd IOT/esp8266
```

### 4. Cấu hình

Mở file `src/config/config.h` và cập nhật các thông tin sau:

```cpp
// WiFi Configuration
static const char* WIFI_SSID = "Your_WiFi_SSID";
static const char* WIFI_PASSWORD = "Your_WiFi_Password";

// MQTT Configuration
static const char* MQTT_SERVER = "192.168.1.10";  // IP của MQTT broker
const int MQTT_PORT = 1883;
static const char* MQTT_USER = "";  // Nếu có authentication
static const char* MQTT_PASS = "";
```

### 5. Upload code

1. Kết nối ESP8266 vào máy tính qua USB
2. Chọn Board: Tools → Board → NodeMCU 1.0 (ESP-12E Module) hoặc Wemos D1 R2 & mini
3. Chọn Port: Tools → Port → COMx (Windows) hoặc /dev/ttyUSBx (Linux)
4. Upload: Sketch → Upload

## ⚙️ Cấu hình

### Các thông số có thể tùy chỉnh trong `config.h`:

```cpp
// Sensor Reading Interval
#define SENSOR_READ_INTERVAL  3000    // ms (3 giây)

// Temperature Thresholds
#define TEMP_THRESHOLD_HIGH    36.0   // °C
#define TEMP_CHANGE_THRESHOLD  0.5    // °C
#define HUMIDITY_CHANGE_THRESHOLD 2.0  // %

// Motion LED Timeout
#define MOTION_LED_TIMEOUT     120000  // ms (2 phút)
```

## 📁 Cấu trúc dự án

```
esp8266/
├── esp8266.ino              # File sketch chính
├── README.md                # File này
└── src/                     # Thư mục chứa các module
    ├── config/
    │   ├── config.h         # Cấu hình hệ thống
    │   └── mqtt_topics.h     # Định nghĩa MQTT topics
    ├── wifi/
    │   ├── wifi_manager.h
    │   └── wifi_manager.cpp # Quản lý kết nối WiFi
    ├── mqtt/
    │   ├── mqtt_manager.h
    │   └── mqtt_manager.cpp # Quản lý MQTT & Discovery
    ├── sensors/
    │   ├── sensor_manager.h
    │   └── sensor_manager.cpp # Đọc DHT11 & SR501
    ├── actuators/
    │   ├── actuator_manager.h
    │   └── actuator_manager.cpp # Điều khiển LED & Buzzer
    └── utils/
        ├── utils.h
        └── utils.cpp         # Các hàm tiện ích
```

## 📡 MQTT Topics

### Discovery Topics (Auto-configuration)
- `homeassistant/sensor/esp8266_smarthome/temperature/config`
- `homeassistant/sensor/esp8266_smarthome/humidity/config`
- `homeassistant/binary_sensor/esp8266_smarthome/motion/config`
- `homeassistant/switch/esp8266_smarthome/led/config`
- `homeassistant/switch/esp8266_smarthome/buzzer/config`

### State Topics
- `homeassistant/sensor/esp8266_smarthome/temperature/state`
- `homeassistant/sensor/esp8266_smarthome/humidity/state`
- `homeassistant/binary_sensor/esp8266_smarthome/motion/state`
- `homeassistant/switch/esp8266_smarthome/led/state`
- `homeassistant/switch/esp8266_smarthome/buzzer/state`

### Command Topics
- `homeassistant/switch/esp8266_smarthome/led/set` (ON/OFF/AUTO)
- `homeassistant/switch/esp8266_smarthome/buzzer/set` (ON/OFF)

## 🏠 Home Assistant Integration

### Tự động phát hiện (MQTT Discovery)

Project sử dụng **MQTT Discovery**, nghĩa là Home Assistant sẽ tự động phát hiện và cấu hình các entities mà không cần khai báo thủ công trong `configuration.yaml`.

**Yêu cầu:**
- Home Assistant đã cấu hình MQTT integration
- MQTT Discovery được bật (mặc định đã bật)

**Sau khi ESP8266 kết nối:**
1. Mở Home Assistant → Settings → Devices & Services
2. Tìm device "Smart Home ESP8266"
3. Các entities sẽ tự động xuất hiện:
   - `sensor.temperature`
   - `sensor.humidity`
   - `binary_sensor.motion`
   - `switch.led_light`
   - `switch.buzzer`

### Điều khiển LED

- **ON**: Bật đèn (tắt chế độ tự động)
- **OFF**: Tắt đèn (tắt chế độ tự động)
- **AUTO**: Bật chế độ tự động (đèn tự bật khi có chuyển động)

### Chế độ tự động

Khi LED ở chế độ AUTO:
- Tự động bật khi phát hiện chuyển động
- Tự động tắt sau 2 phút không có chuyển động

## 🚀 Sử dụng

### Serial Monitor

Mở Serial Monitor (115200 baud) để xem log:

```
╔════════════════════════════════════════╗
║   SMART HOME - HOME ASSISTANT MQTT     ║
╚════════════════════════════════════════╝

✓ GPIO pins initialized
✓ DHT11 sensor initialized
📶 Connecting to WiFi: J19
✓ WiFi connected successfully!
  IP Address: 192.168.1.100
✓ MQTT client configured
📡 Attempting MQTT connection...
✓ Connected!
🔍 Publishing MQTT Discovery configurations...
  ✓ Temperature sensor discovery published
  ✓ Humidity sensor discovery published
  ...
```

### Kiểm tra hoạt động

1. **Kiểm tra WiFi**: Serial Monitor sẽ hiển thị IP address
2. **Kiểm tra MQTT**: Kiểm tra kết nối MQTT broker
3. **Kiểm tra Sensors**: Xem giá trị nhiệt độ, độ ẩm trên Serial Monitor
4. **Test Motion**: Di chuyển trước SR501, đèn sẽ tự động bật (nếu ở chế độ AUTO)
5. **Test Home Assistant**: Mở Home Assistant và kiểm tra các entities

## 🔍 Troubleshooting

### ESP8266 không kết nối WiFi

- Kiểm tra SSID và password trong `config.h`
- Đảm bảo WiFi 2.4GHz (ESP8266 không hỗ trợ 5GHz)
- Kiểm tra khoảng cách đến router

### Không kết nối được MQTT

- Kiểm tra IP của MQTT broker trong `config.h`
- Đảm bảo MQTT broker đang chạy
- Kiểm tra firewall/port 1883
- Kiểm tra MQTT_USER và MQTT_PASS nếu có authentication

### Home Assistant không phát hiện device

- Kiểm tra MQTT integration đã được cấu hình trong Home Assistant
- Kiểm tra MQTT Discovery được bật
- Xem log Home Assistant để tìm lỗi
- Thử restart Home Assistant

### DHT11 không đọc được giá trị

- Kiểm tra kết nối chân Data (GPIO14/D5)
- Kiểm tra pull-up resistor 10kΩ
- Đảm bảo DHT11 được cấp nguồn 3.3V
- Chờ 2-3 giây sau khi khởi động để DHT11 ổn định

### Relay không hoạt động

- Kiểm tra logic relay: `RELAY_ON = LOW`, `RELAY_OFF = HIGH`
- Kiểm tra nguồn cấp cho relay module
- Kiểm tra kết nối GPIO (GPIO5 cho LED, GPIO4 cho Buzzer)

### Serial Monitor không hiển thị

- Kiểm tra baud rate: 115200
- Kiểm tra kết nối USB
- Thử reset ESP8266

## 📝 License

MIT License

## 👤 Author

Smart Home Project

## 🙏 Acknowledgments

- ESP8266 Community
- Home Assistant Team
- Adafruit (DHT library)

---

**Version**: 1.0.0  
**Last Updated**: 2024

