void setup() {
  Serial.begin(9600);  // For debugging (optional)
  pinMode(8, OUTPUT); // LED on Pin 8 
}

void loop() {
  if (Serial.available()) {  // If data received from ESP32
    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove extra spaces/newlines

    if (command == "ON") {
      digitalWrite(8, HIGH);  
      Serial.println("LED ON");
    } 
    else if (command == "OFF") {
      digitalWrite(8, LOW);   // Turn LED OFF
      Serial.println("LED OFF");
    }
  }
}
