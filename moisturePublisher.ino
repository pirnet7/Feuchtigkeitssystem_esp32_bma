#include <WiFi.h>
#include <MQTT.h>

// ========== CONFIGURATION ==========

// WiFi credentials
#define WIFI_SSID "bruphone"
#define WIFI_PASSWORD "12345678"

// MQTT broker configuration
#define MQTT_CLIENT_ID "BBW_ESP32_BV"
#define MQTT_BROKER_URL "public.cloud.shiftr.io"
#define MQTT_BROKER_USER "public"
#define MQTT_BROKER_PASSWORD "public"
#define MQTT_TOPIC "BBW/SoilMoisture" // Topic für Feuchtigkeit

// Sensor configuration
const int SOIL_SENSOR_PIN = 2; // ADC pin for sensor
const int ADC_DRY = 3500;      // Calibration value: dry soil
const int ADC_WET = 1000;      // Calibration value: wet soil
const int NUM_SAMPLES = 10;    // Number of samples for averaging

// Thresholds for classification
const int THRESHOLD_DRY = 20;      // below 20% = "dry"
const int THRESHOLD_CRITICAL = 35; // 20-35% = "critical", above = "ok"

// Measurement interval
const unsigned long MEASURE_INTERVAL_MS = 1000; // 1 second

// ========== GLOBAL VARIABLES ==========

WiFiClient wifiNetwork;
MQTTClient mqttClient(256); // max payload 256 bytes
unsigned long lastMeasurement = 0;

// ========== SETUP ==========

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("========================================");
  Serial.println("Plant Sitter - Soil Moisture System");
  Serial.println("========================================");

  // Establish connections
  connectWiFi();
  connectMQTT();

  Serial.println("System ready!");
  Serial.println("========================================");
}

// ========== MAIN LOOP ==========

void loop()
{
  // Update MQTT client
  mqttClient.loop();

  // Check connections and reconnect if necessary
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi connection lost! Reconnecting...");
    connectWiFi();
  }

  if (!mqttClient.connected())
  {
    Serial.println("MQTT connection lost! Reconnecting...");
    connectMQTT();
  }

  // Perform measurements at defined interval
  if (millis() - lastMeasurement >= MEASURE_INTERVAL_MS)
  {
    lastMeasurement = millis();
    publishMoisture();
  }
}

// ========== WIFI CONNECTION ==========

void connectWiFi()
{
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    attempts++;

    if (attempts > 60)
    { // 30 seconds timeout
      Serial.println();
      Serial.println("ERROR: WiFi connection failed!");
      Serial.println("Retrying in 5 seconds...");
      delay(5000);
      attempts = 0;
    }
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

// ========== MQTT CONNECTION ==========

void connectMQTT()
{
  Serial.println();
  Serial.print("Connecting to MQTT broker: ");
  Serial.println(MQTT_BROKER_URL);

  mqttClient.begin(MQTT_BROKER_URL, wifiNetwork);

  int attempts = 0;
  while (!mqttClient.connect(MQTT_CLIENT_ID, MQTT_BROKER_USER, MQTT_BROKER_PASSWORD))
  {
    Serial.print(".");
    delay(500);
    attempts++;

    if (attempts > 40)
    { // 20 seconds timeout
      Serial.println();
      Serial.println("ERROR: MQTT connection failed!");
      Serial.println("Retrying in 5 seconds...");
      delay(5000);
      attempts = 0;
    }
  }

  Serial.println();
  Serial.print("MQTT connected! Client ID: ");
  Serial.println(MQTT_CLIENT_ID);
  Serial.print("Publishing to topic: ");
  Serial.println(MQTT_TOPIC);
}

// ========== SENSOR FUNCTIONS ==========

int readMoistureRaw()
{
  long sum = 0;

  // Multiple measurements for stable average
  for (int i = 0; i < NUM_SAMPLES; i++)
  {
    sum += analogRead(SOIL_SENSOR_PIN);
    delay(10); // Short pause between measurements
  }

  return sum / NUM_SAMPLES;
}

int convertToPercent(int raw)
{
  // Conversion: ADC_DRY = 0%, ADC_WET = 100%
  int percent = map(raw, ADC_DRY, ADC_WET, 0, 100);

  // Constrain values to valid range
  percent = constrain(percent, 0, 100);

  return percent;
}

String classifyMoisture(int percent)
{
  if (percent < THRESHOLD_DRY)
  {
    return "dry";
  }
  else if (percent < THRESHOLD_CRITICAL)
  {
    return "critical";
  }
  else
  {
    return "ok";
  }
}

// ========== MQTT PUBLISH ==========

void publishMoisture()
{
  // Read sensor
  int rawValue = readMoistureRaw();
  int percentValue = convertToPercent(rawValue);
  String state = classifyMoisture(percentValue);

  // Create JSON payload
  String payload = "{\"raw\":";
  payload += rawValue;
  payload += ",\"percent\":";
  payload += percentValue;
  payload += ",\"state\":\"";
  payload += state;
  payload += "\"}";

  // Send to MQTT broker
  bool success = mqttClient.publish(MQTT_TOPIC, payload);

  // Debug output
  Serial.println();
  Serial.println("--- New Measurement ---");
  Serial.print("Raw value: ");
  Serial.println(rawValue);
  Serial.print("Moisture: ");
  Serial.print(percentValue);
  Serial.println(" %");
  Serial.print("Status: ");
  Serial.println(state);
  Serial.print("MQTT Publish: ");
  Serial.println(success ? "SUCCESS" : "ERROR");
  Serial.print("Payload: ");
  Serial.println(payload);
  Serial.println("----------------");
}
