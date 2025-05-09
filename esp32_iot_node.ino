#include <WiFiManager.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Pins and Constants
#define DHTPIN 4
#define DHTTYPE DHT11
#define LDR_PIN 36         // ADC1 channel (GPIO 36 or 39 recommended)
#define HX_SCK 18
#define HX_DOUT 19
#define DEVICE_ID "1"

// API Endpoint
const char* serverName = "https:example.com";  // Replace with your actual API

// Sensor Objects
DHT dht(DHTPIN, DHTTYPE);


// Read HX710B (used in MPS20N0040D module)
long readHX710B() {
  while (digitalRead(HX_DOUT));
  long count = 0;
  for (int i = 0; i < 24; i++) {
    digitalWrite(HX_SCK, HIGH);
    delayMicroseconds(1);
    count = count << 1;
    digitalWrite(HX_SCK, LOW);
    delayMicroseconds(1);
    if (digitalRead(HX_DOUT)) count++;
  }

  // 25th pulse
  digitalWrite(HX_SCK, HIGH);
  delayMicroseconds(1);
  digitalWrite(HX_SCK, LOW);
  delayMicroseconds(1);

  // Convert signed 24-bit to long
  if (count & 0x800000) count |= 0xFF000000;

  return count;
}




void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(HX_SCK, OUTPUT);
  pinMode(HX_DOUT, INPUT);
  pinMode(LDR_PIN, INPUT);

  // ===== WiFiManager with Static AP IP =====
  WiFiManager wifiManager;
  IPAddress apIP(10, 0, 0, 1);
  IPAddress gateway(10, 0, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  wifiManager.setAPStaticIPConfig(apIP, gateway, subnet);

  if (!wifiManager.autoConnect("ESP32_AP", "esp32pass")) {
    Serial.println("Failed to connect. Restarting...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("WiFi connected!");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float temperature = dht.readTemperature();
  Serial.print("Temp:");
  Serial.println(temperature);
  float humidity = dht.readHumidity();
  Serial.print("Humidity:");
  Serial.println(humidity);
  
  int ldrValue = analogRead(LDR_PIN);
  
  long pressureRaw = readHX710B();
  

  // Convert raw pressure to usable units (approximation may vary)
  float pressure = (8388608 - pressureRaw ) / 10000.0;  // Adjust scale as per your module's calibration
Serial.print("Pressure:");
  Serial.println(pressure);

  

  // Determine day or night (adjust threshold based on testing)
  String timeOfDay = ldrValue > 2000 ? "day" : "night";
  
  Serial.print("ldr value:");
  Serial.println(ldrValue);
  Serial.print(timeOfDay);
  

  // Prepare JSON
  StaticJsonDocument<256> jsonDoc;
  jsonDoc["device_id"] = DEVICE_ID;
  jsonDoc["temprature"] = temperature;
  jsonDoc["humidity"] = humidity;
  jsonDoc["pressure"] = pressure;
  jsonDoc["time"] = timeOfDay;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Send POST request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      Serial.printf("POST success, code: %d\n", httpResponseCode);
    } else {
      Serial.printf("POST failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }

  delay(60000);  // Send data every 1 min
}
