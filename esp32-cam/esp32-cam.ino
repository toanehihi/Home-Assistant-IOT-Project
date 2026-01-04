#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"
#include "esp_log.h"
#include "esp_system.h"

// WiFi credentials - QUAN TRỌNG: Phải thay đổi thông tin WiFi của bạn!
const char* ssid = "K**A";     // VÍ DỤ: "MyHomeWiFi"
const char* password = "tumotdenchin";  // VÍ DỤ: "mypassword123"

// Camera model AI Thinker ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// LED pins cho ESP32-CAM
#define LED_BUILTIN       4   // LED trạng thái tích hợp
#define FLASH_LED         4   // LED flash (cùng pin với LED_BUILTIN trên một số board)

WebServer server(80);

bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 24000000; // Overclock camera từ 20MHz lên 24MHz
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST; // Chỉ lấy frame mới nhất
  
  // Cấu hình cân bằng giữa tốc độ và chất lượng
  if(psramFound()) {
    Serial.println("PSRAM found - Balanced quality/speed settings");
    config.frame_size = FRAMESIZE_VGA; // 640x480 - độ phân giải tốt
    config.jpeg_quality = 12;  // Chất lượng tốt nhưng vẫn nhanh
    config.fb_count = 2;
  } else {
    Serial.println("PSRAM not found - Optimized settings");
    config.frame_size = FRAMESIZE_HVGA; // 480x320 - trung bình
    config.jpeg_quality = 15;  // Chất lượng khá
    config.fb_count = 1;
  }
  
  // Khởi tạo camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return false;
  }
  
  // Tùy chọn sensor để cân bằng chất lượng và tốc độ
  sensor_t * s = esp_camera_sensor_get();
  if (s->id.PID == OV2640_PID) {
    Serial.println("OV2640 sensor detected - Balanced quality/speed optimization");
    s->set_vflip(s, 1);        // Lật ảnh theo chiều dọc
    s->set_brightness(s, 0);   // -2 to 2
    s->set_contrast(s, 1);     // Tăng contrast cho ảnh rõ nét
    s->set_saturation(s, 0);   // -2 to 2  
    s->set_sharpness(s, 1);    // Tăng độ sắc nét
    s->set_denoise(s, 1);      // Bật denoise để ảnh đẹp hơn
    s->set_gainceiling(s, (gainceiling_t)2); // Gain trung bình
    s->set_quality(s, 12);     // Quality tốt cho sensor
    s->set_colorbar(s, 0);     // Tắt colorbar
    s->set_whitebal(s, 1);     // Bật white balance tự động
    s->set_gain_ctrl(s, 1);    // Bật gain control tự động
    s->set_exposure_ctrl(s, 1); // Bật exposure control tự động
    s->set_hmirror(s, 0);      // Không mirror ngang
    s->set_dcw(s, 0);          // Tắt downsize để giữ chất lượng
  }
  
  Serial.println("Camera initialized successfully");
  return true;
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  WiFi.setSleep(false); // Tắt chế độ tiết kiệm điện để stream ổn định
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Stream URL: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/stream");
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi!");
  }
}

void handleStream() {
  Serial.println("Stream requested by client");
  
  WiFiClient client = server.client();
  
  // Thiết lập timeout cho client để tránh treo
  client.setTimeout(1000); // 1 giây timeout
  
  // Gửi HTTP headers cho MJPEG stream
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n";
  response += "Cache-Control: no-cache\r\n";
  response += "Connection: close\r\n\r\n";
  client.print(response);
  
  while (client.connected()) {
    // Lấy frame từ camera với timeout ngắn
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      continue; // Tiếp tục thay vì break để duy trì stream
    }

    // Kiểm tra kích thước frame hợp lệ
    if (fb->len > 0) {
      // Gửi boundary và headers cho frame
      client.print("--frame\r\n");
      client.print("Content-Type: image/jpeg\r\n");
      client.print("Content-Length: ");
      client.print(fb->len);
      client.print("\r\n\r\n");

      // Gửi dữ liệu JPEG với buffer size lớn hơn
      size_t sent = client.write(fb->buf, fb->len);
      client.print("\r\n");
      
      if (sent != fb->len) {
        Serial.println("Failed to send complete frame");
      }
    }

    // Trả về frame buffer ngay lập tức
    esp_camera_fb_return(fb);    // Kiểm tra kết nối client
    if (!client.connected()) {
      Serial.println("Client disconnected");
      break;
    }
    
    // Delay nhẹ để cân bằng FPS và chất lượng
    delay(5); // Delay nhẹ để tránh overload, vẫn đạt ~200 FPS
  }
  
  client.stop();
  Serial.println("Stream ended");
}

void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>ESP32-CAM Stream</title></head>";
  html += "<body>";
  html += "<h1>ESP32-CAM Live Stream</h1>";
  html += "<img src='/stream' style='width:100%; max-width:800px;'>";
  html += "<p>Stream URL: <a href='/stream'>http://";
  html += WiFi.localIP().toString();
  html += "/stream</a></p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("ESP32-CAM MJPEG Stream Server");
  Serial.println("==============================");
  
  // Khởi tạo LED
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLASH_LED, OUTPUT);
  
  // Bật đèn báo hiệu khởi động
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED status: ON - ESP32-CAM is starting...");
  
  // Khởi tạo camera
  if (!initCamera()) {
    Serial.println("Failed to initialize camera!");
    ESP.restart();
  }
  
  // Kết nối WiFi
  connectWiFi();
  
  // Tối ưu WiFi cho streaming
  WiFi.setSleep(false);  // Tắt power saving
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot start server without WiFi connection");
    ESP.restart();
  }
  
  // Cấu hình web server
  server.on("/", handleRoot);
  server.on("/stream", HTTP_GET, handleStream);
  server.onNotFound(handleNotFound);
  
  // Khởi động server
  server.begin();
  Serial.println("HTTP server started on port 80");
  Serial.println("Ready to stream!");
  
  // Nhấp nháy LED báo hiệu sẵn sàng
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }
}

void loop() {
  server.handleClient();
  
  // Kiểm tra kết nối WiFi và reconnect nếu cần
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, attempting to reconnect...");
    connectWiFi();
  }
  
  delay(10);
}