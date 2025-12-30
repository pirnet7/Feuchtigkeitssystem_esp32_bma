#include <Arduino.h>
#include "config.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("Nano C6 - Soil Moisture Test");
}

void loop()
{
  int sensorValue = analogRead(SOIL_SENSOR_PIN);
  int moisturePercent = 100L * (SOIL_DRY_VALUE - sensorValue) / (SOIL_DRY_VALUE - SOIL_WET_VALUE);
  moisturePercent = constrain(moisturePercent, 0, 100);

  Serial.print("Raw value: ");
  Serial.print(sensorValue);
  Serial.print("\tMoisture: ");
  Serial.print(moisturePercent);
  Serial.println(" %");

  delay(MEASURE_INTERVAL_MS);
}
