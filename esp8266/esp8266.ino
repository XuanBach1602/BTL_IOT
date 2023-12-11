#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
SoftwareSerial arduinoSerial(D1, D2);

const char* ssid = "No name";
const char* password = "77777777";
String locality, city,latitude,longitude;



void setup() {
  Serial.begin(4800);
  arduinoSerial.begin(4800);
  connectWiFi();
  testConnection();
}

void loop() {

  if (arduinoSerial.available()) {
    delay(5000);
    Serial.println("received ....");
    // String receivedData = arduinoSerial.readStringUntil('\n'); // Đọc dữ liệu từ Serial
    String receivedData = arduinoSerial.readStringUntil('\n');
    Serial.println(receivedData);
    Serial.print("longitude: ");
    Serial.println(longitude);
    Serial.print("latitude: ");
    Serial.println(latitude);
    // Tạo một đối tượng JSON để lưu trữ dữ liệu
    DynamicJsonDocument jsonDoc(400); // Đổi kích thước tùy thuộc vào độ lớn dữ liệu

    // Deserialize dữ liệu từ chuỗi JSON nhận được từ Serial
    DeserializationError error = deserializeJson(jsonDoc, receivedData);

    // Kiểm tra lỗi khi deserialize
    // if (error) {
    //   Serial.print("deserializeJson() failed: ");
    //   Serial.println(error.c_str());
    //   return;
    // }

    // // In ra dữ liệu JSON nhận được từ Serial
    // serializeJsonPretty(jsonDoc, Serial);
    Serial.println(); // In một dòng mới
  }
}

void connectWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void testConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    const char* host = "api.bigdatacloud.net";
    const int httpPort = 80;

    Serial.println("Starting request...");

    if (!client.connect(host, httpPort)) {
      Serial.println("Connection failed");
      return;
    }

    client.print(String("GET /data/reverse-geocode-client?localityLanguage=vi HTTP/1.1\r\n") +
                 "Host: " + String(host) + "\r\n" +
                 "Connection: close\r\n\r\n");

    // Serial.println("Request sent");

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }

    String response = client.readString();
    // Serial.println("Response received:");
    // Serial.println(response);

    // Parse JSON response
    const size_t capacity = JSON_OBJECT_SIZE(10) + 400;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, response);

    // Extract 'locality' field from JSON
    locality = String(doc["locality"]);
    latitude = String(doc["latitude"]);
    longitude = String(doc["longitude"]);
    city = String(doc["city"]);
    Serial.print("Address: "+locality +", " + city);
   
    
    client.stop();
  }
}
