#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Cấu hình WiFi & MQTT
const char* ssid = "J19";
const char* password = "hoangchimbe";
const char* mqtt_server = "172.26.48.1";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(2, DHT11);  // DHT11 ở GPIO2

// Relay pins
int relays[] = { 5, 4, 12, 13 };  // D1-D4
int motionPin = 14;               // D5

void setup() {
  Serial.begin(9600);
  Serial.println("\nESP8266 Starting...");
  // Khởi tạo relay
  for (int i = 0; i < 4; i++) {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], HIGH);  // Relay LOW active
  }

  pinMode(motionPin, INPUT);
  dht.begin();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected successfully!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Điều khiển relay
  if (String(topic) == "home/relay1") {
    digitalWrite(relays[0], message == "ON" ? LOW : HIGH);
  }
  // Thêm relay 2,3,4 tương tự
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      client.subscribe("home/relay1");
      client.subscribe("home/relay2");
      client.subscribe("home/relay3");
      client.subscribe("home/relay4");
    } else {
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Đọc cảm biến mỗi 5s
  static unsigned long lastMsg = 0;
  unsigned long now = millis();

  if (now - lastMsg > 5000) {
    lastMsg = now;

    // DHT11
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (!isnan(temp)) {
      client.publish("home/temperature", String(temp).c_str());
      client.publish("home/humidity", String(hum).c_str());
    }

    // Motion sensor
    int motion = digitalRead(motionPin);
    client.publish("home/motion", motion ? "ON" : "OFF");
  }
}