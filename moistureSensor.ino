const int soilSensorPin = 2; // ADC-Pin auf Nano C6

// Kalibrierungswerte
const int dryValue = 3500;  // Sensor in Luft
const int wetValue = 1000;  // Sensor in Wasser

void setup() {
  Serial.begin(115200);
  Serial.println("Nano C6 - Soil Moisture Test");
}

void loop() {
  int sensorValue = analogRead(soilSensorPin);

  int moisturePercent = map(sensorValue, dryValue, wetValue, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  Serial.print("Rohwert: ");
  Serial.print(sensorValue);
  Serial.print("\tFeuchtigkeit: ");
  Serial.print(moisturePercent);
  Serial.println(" %");

  delay(1000);
}
