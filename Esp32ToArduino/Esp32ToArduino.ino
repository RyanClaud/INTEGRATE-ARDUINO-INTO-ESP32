#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// WiFi Credentials
const char* ssid = "Private";
const char* password = "p@ssW0rd";

// Firebase URL
String firebaseUrl = "https://integrate-arduino-into-esp32-default-rtdb.firebaseio.com/ledStatus.json";

// Web Server
WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize UART for Arduino communication
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  // Define routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/on", handleLEDOn);
  server.on("/off", handleLEDOff);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}


void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta name='viewport' content='width=device-width, initial-scale=1'/>";
  html += "<style>";
  html += "body { text-align: center; font-family: Arial, sans-serif; background-color: #f2f2f2; margin-top: 50px; }";
  html += "h1 { color: #333; }";
  html += "button { padding: 15px 30px; font-size: 18px; margin: 10px; border: none; border-radius: 8px; cursor: pointer; transition: 0.3s; }";
  html += ".on-btn { background-color: #4CAF50; color: white; }";
  html += ".off-btn { background-color: #f44336; color: white; }";
  html += ".on-btn:hover { background-color: #45a049; }";
  html += ".off-btn:hover { background-color: #e53935; }";
  html += ".status { margin-top: 20px; font-size: 20px; color: green; }";
  html += "</style></head><body>";
  html += "<h1>ESP32 LED Control</h1>";

  // Buttons
  html += "<p><button class=\"on-btn\" onclick=\"sendCommand('on')\">Turn ON</button></p>";
  html += "<p><button class=\"off-btn\" onclick=\"sendCommand('off')\">Turn OFF</button></p>";

  // Status Message
  html += "<div id=\"statusMessage\" class=\"status\"></div>";

  // AJAX Script
  html += "<script>";
  html += "function sendCommand(cmd) {";
  html += "  fetch('/' + cmd)";
  html += "    .then(response => response.text())";
  html += "    .then(data => {";
  html += "      document.getElementById('statusMessage').innerText = data;";
  html += "    })";
  html += "    .catch(error => {";
  html += "      document.getElementById('statusMessage').innerText = 'Error sending command';";
  html += "    });";
  html += "}";
  html += "</script>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleLEDOn() {
  Serial2.println("ON"); // Tell Arduino to turn LED ON
  updateFirebase("ON");
  server.send(50, "text/plain", "LED turned ON");
}

void handleLEDOff() {
  Serial2.println("OFF"); // Tell Arduino to turn LED OFF
  updateFirebase("OFF");
  server.send(50, "text/plain", "LED turned OFF");
}

// POST value to Firebase
void updateFirebase(String status) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(firebaseUrl);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST("{\"status\":\"" + status + "\"}");

    if (httpResponseCode > 0) {
      Serial.printf("Firebase Response Code: %d\n", httpResponseCode);
    } else {
      Serial.println("Error sending to Firebase");
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
