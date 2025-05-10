#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "B315_20312";
const char* password = "DEEABE7H406";

// Firebase details
const String firebaseHost = "https://integrate-arduino-into-esp32-default-rtdb.firebaseio.com/";
const String firebasePath = "/sensorData.json";

// Web server instance
WebServer server(80);

// Sensor data variables
String dhtTemp = "--";
String dhtHum = "--";
String lm35Temp = "--";
String ldr = "--";
String waterDetected = "--";

// Function prototype
void handleData();

// Send data to Firebase
void sendToFirebase() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(firebaseHost + firebasePath);
    http.addHeader("Content-Type", "application/json");

    String json = "{";
    json += "\"dhtTemp\":" + dhtTemp + ",";
    json += "\"dhtHum\":" + dhtHum + ",";
    json += "\"lm35Temp\":" + lm35Temp + ",";
    json += "\"ldr\":" + ldr + ",";
    json += "\"waterDetected\":\"" + waterDetected + "\"";
    json += "}";

    int httpCode = http.PUT(json);
    Serial.println("Firebase Response: " + String(httpCode));
    http.end();
  }
}

// Extract a value from the incoming serial string
String getValue(String data, String key) {
  int start = data.indexOf(key + ":");
  if (start == -1) return "";
  start += key.length() + 1;
  int end = data.indexOf(',', start);
  if (end == -1) end = data.length();
  return data.substring(start, end);
}

// HTML page handler
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <title>ESP32 Sensor Dashboard</title>
      <style>
        body {
          font-family: Arial, sans-serif;
          background: #f0f0f0;
          margin: 0;
          padding: 20px;
          text-align: center;
        }
        h1 { margin-bottom: 30px; }
        .container {
          display: flex;
          flex-wrap: wrap;
          justify-content: center;
        }
        .card {
          background: white;
          border-radius: 12px;
          box-shadow: 0 4px 8px rgba(0,0,0,0.1);
          width: 200px;
          margin: 10px;
          padding: 20px;
          box-sizing: border-box;
          transition: 0.3s;
        }
        .card:hover {
          transform: scale(1.05);
        }
        .icon {
          font-size: 30px;
          margin-bottom: 10px;
          color: #007BFF;
        }
        .label {
          font-size: 16px;
          color: #555;
        }
        .value {
          font-size: 28px;
          font-weight: bold;
          color: #007BFF;
        }
        @media (max-width: 600px) {
          .card {
            width: 90%;
          }
        }
      </style>
      <script>
        function fetchData() {
          fetch("/data")
            .then(res => res.json())
            .then(data => {
              document.getElementById("temp").innerText = data.dhtTemp;
              document.getElementById("hum").innerText = data.dhtHum;
              document.getElementById("lm35").innerText = data.lm35Temp;
              document.getElementById("ldr").innerText = data.ldr;
              document.getElementById("water").innerText = data.waterDetected;
            });
        }
        setInterval(fetchData, 2000);
        window.onload = fetchData;
      </script>
      <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.0/css/all.min.css">
    </head>
    <body>
      <h1>ESP32 Sensor Dashboard</h1>
      <div class="container">
        <div class="card">
          <div class="icon"><i class="fas fa-thermometer-half"></i></div>
          <div class="label">DHT Temp (°C)</div>
          <div class="value" id="temp">--</div>
        </div>
        <div class="card">
          <div class="icon"><i class="fas fa-water"></i></div>
          <div class="label">Humidity (%)</div>
          <div class="value" id="hum">--</div>
        </div>
        <div class="card">
          <div class="icon"><i class="fas fa-temperature-high"></i></div>
          <div class="label">LM35 Temp (°C)</div>
          <div class="value" id="lm35">--</div>
        </div>
        <div class="card">
          <div class="icon"><i class="fas fa-lightbulb"></i></div>
          <div class="label">LDR Value</div>
          <div class="value" id="ldr">--</div>
        </div>
        <div class="card">
          <div class="icon"><i class="fas fa-tint"></i></div>
          <div class="label">Water Leak</div>
          <div class="value" id="water">--</div>
        </div>
      </div>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// JSON data handler
void handleData() {
  String json = "{";
  json += "\"dhtTemp\":\"" + dhtTemp + "\",";
  json += "\"dhtHum\":\"" + dhtHum + "\",";
  json += "\"lm35Temp\":\"" + lm35Temp + "\",";
  json += "\"ldr\":\"" + ldr + "\",";
  json += "\"waterDetected\":\"" + waterDetected + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.startsWith("TEMP:")) {
      dhtTemp = getValue(input, "TEMP");
      dhtHum = getValue(input, "HUM");
      lm35Temp = getValue(input, "LM35");
      ldr = getValue(input, "LDR");
      waterDetected = getValue(input, "WATER");

      Serial.println("Received: " + input);
      sendToFirebase();
    }
  }
}
