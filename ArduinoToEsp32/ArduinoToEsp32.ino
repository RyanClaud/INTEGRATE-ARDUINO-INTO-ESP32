#include <DHT.h>

#define DHTPIN 3
#define DHTTYPE DHT11
#define LM35PIN A0
#define LDRPIN A1
#define WATERPIN 4
#define RELAY_PIN 5  // Relay control pin

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(WATERPIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);  // Set relay pin as output
}

void loop() {
  // Read from DHT11
  float dhtTemp = dht.readTemperature();
  float dhtHum = dht.readHumidity();

  // Correct LM35 calculation (10mV per °C)
  int lm35Val = analogRead(LM35PIN);                // Raw analog value (0–1023)
  float voltage = lm35Val * (5.0 / 1023.0);          // Convert to voltage
  float lm35Temp = voltage * 100.0;                  // Convert to °C

  // Read LDR and Water Sensor
  int ldrVal = analogRead(LDRPIN);
  int waterVal = digitalRead(WATERPIN);              // HIGH = dry, LOW = water detected

  // Relay control based on water detection
  if (waterVal == HIGH) {
    digitalWrite(RELAY_PIN, HIGH);  // No water: turn on relay
  } else {
    digitalWrite(RELAY_PIN, LOW);   // Water detected: turn off relay
  }

  // Send data to ESP32 via Serial in key:value format
  Serial.print("TEMP:");
  Serial.print(dhtTemp);
  Serial.print(",HUM:");
  Serial.print(dhtHum);
  Serial.print(",LM35:");
  Serial.print(lm35Temp);
  Serial.print(",LDR:");
  Serial.print(ldrVal);
  Serial.print(",WATER:");
  Serial.println(waterVal == HIGH ? "YES" : "NO");

  delay(2000);  // Wait 2 seconds
}
