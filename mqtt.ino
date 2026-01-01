// ========================================
// BEISPIEL / TEST FILE
// ========================================
// Diese Datei dient nur zu Testzwecken.
// Für das Produktivsystem verwende: moisturePublisher.ino
//
// MQTT publish example
//
// mqtt lib:      https://github.com/256dpi/arduino-mqtt
// mqtt provider: https://www.shiftr.io/try/
//
//-------------------------------------------------------------------
#include <WiFi.h>
#include <MQTT.h>
#include "config.h"

//-------------------------------------------------------------------
WiFiClient wifinetwork;
MQTTClient mqttclient;

//-------------------------------------------------------------------
void setup(void)
{
  //---
  Serial.begin(115200);

  //---
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to network ");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(250);
  }
  Serial.println(" Done");

  //---
  mqttclient.begin(wifinetwork);

  mqttclient.setHost(MQTT_BROKER_URL);
  Serial.print("Connecting to MQTT broker ");

  while (mqttclient.connect(MQTT_CLIENT_ID, MQTT_BROKER_USER, MQTT_BROKER_PASSWORD) == false)
  {
    Serial.print(".");
    delay(250);
  }
  Serial.println(" Done");
}

//-------------------------------------------------------------------
void loop(void)
{
  static unsigned long lastPublish = 0;

  mqttclient.loop();

  //--- Reconnect bei Verbindungsverlust
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi lost! Reconnecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(250);
    }
    Serial.println(" WiFi reconnected");
  }

  if (!mqttclient.connected())
  {
    Serial.println("MQTT lost! Reconnecting...");
    while (!mqttclient.connect(MQTT_CLIENT_ID, MQTT_BROKER_USER, MQTT_BROKER_PASSWORD))
    {
      Serial.print(".");
      delay(250);
    }
    Serial.println(" MQTT reconnected");
  }

  //---
  // T=1000ms (1 Hz)
  if ((millis() - lastPublish) > 1000)
  {
    lastPublish = millis();

    String topic = MQTT_TOPIC;
    String payload = "Hello World";
    mqttclient.publish(topic, payload);

    Serial.println("Message published :-)");
  }
}
