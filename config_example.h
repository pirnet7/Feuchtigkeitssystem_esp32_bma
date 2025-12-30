// Example configuration for the soil moisture publisher.
// Copy this file to config.h and fill in your real credentials and calibration.

#ifndef PLANT_SITTER_CONFIG_EXAMPLE_H
#define PLANT_SITTER_CONFIG_EXAMPLE_H

// WiFi credentials
#define WIFI_SSID        "YOUR_WIFI_SSID"
#define WIFI_PASSWORD    "YOUR_WIFI_PASSWORD"

// MQTT broker configuration
#define MQTT_CLIENT_ID   "MY_ESP32_CLIENT"
#define MQTT_BROKER_URL  "public.cloud.shiftr.io"
#define MQTT_BROKER_USER "public"
#define MQTT_BROKER_PASSWORD "public"
#define MQTT_TOPIC       "BBW/SoilMoisture"

// Sensor configuration
#define SOIL_SENSOR_PIN  1
#define SOIL_DRY_VALUE   3900  // ADC reading when the sensor is dry
#define SOIL_WET_VALUE   1200  // ADC reading when the sensor is wet

// Moisture thresholds
#define THRESHOLD_DRY       20
#define THRESHOLD_CRITICAL  35

// Measurement interval (ms)
#define MEASURE_INTERVAL_MS 1000

#endif  // PLANT_SITTER_CONFIG_EXAMPLE_H
