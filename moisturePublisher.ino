#include <WiFi.h>
#include <MQTT.h>

// --------- WiFi ----------
#define WIFI_SSID  "bruphone"
#define WIFI_KEY   "12345678"

// --------- MQTT ----------
#define MQTT_CLIENT_ID  "BBW_ESP32_BV"
#define MQTT_BROKER_URL            "public.cloud.shiftr.io"
#define MQTT_BROKER_USER_NAME      "public"
#define MQTT_BROKER_USER_PASSWORD  "public"
#define MQTT_TOPIC "BBW/SoilMoisture"  // Topic für Feuchtigkeit

WiFiClient wifinetwork;
MQTTClient mqttclient(256); // max payload 256 bytes

// --------- Sensor ----------
const int soilSensorPin = 2; // ADC-Pin für Nano C6

// Kalibrierung: trockener Sensor und Wasser
const int dryValue = 3500;
const int wetValue = 1000;

void setup() {
  Serial.begin(115200);

  // --------- WiFi ----------
  WiFi.begin(WIFI_SSID, WIFI_KEY);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Connected!");

  // --------- MQTT ----------
  mqttclient.begin(MQTT_BROKER_URL, wifinetwork);
  Serial.print("Connecting to MQTT broker");
  while (!mqttclient.connect(MQTT_CLIENT_ID, MQTT_BROKER_USER_NAME, MQTT_BROKER_USER_PASSWORD)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Connected to MQTT broker!");
}

void loop() {
  mqttclient.loop();

  static uint32_t lastTime = 0;
  if (millis() - lastTime > 1000) { // jede Sekunde
    lastTime = millis();

    int sensorValue = analogRead(soilSensorPin);
    int moisturePercent = map(sensorValue, dryValue, wetValue, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);

    String payload = String(moisturePercent);

    mqttclient.publish(MQTT_TOPIC, payload);
    Serial.print("Feuchtigkeit: ");
    Serial.print(moisturePercent);
    Serial.println("% -> published!");
  }
}
