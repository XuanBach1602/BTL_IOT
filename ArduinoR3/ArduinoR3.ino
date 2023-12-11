#include <SoftwareSerial.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int MQ7_PIN = A0;
const int MQ135_PIN = A1;
// const int NEO_RX = 5;
// const int NEO_TX = 6;
// SoftwareSerial gpsSerial(NEO_RX, NEO_TX);
SoftwareSerial espSerial(10, 11);
StaticJsonDocument<200> jsonDoc;

void setup() {
  Serial.begin(9600);
  dht.begin();
  espSerial.begin(4800);
  // gpsSerial.begin(9600);
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  int mq7Value = analogRead(MQ7_PIN); 
  float coConcentration = map(mq7Value, 0, 1023, 0, 100);

  int mq135Value = analogRead(MQ135_PIN);
  float nh3Concentration = map(mq135Value, 0, 1023, 0, 100);
  float co2Concentration = map(mq135Value, 0, 1023, 0, 5000);

  jsonDoc["humidity"] = humidity;
  jsonDoc["temperature"] = temperature;
  jsonDoc["CO_concentration"] = coConcentration;
  jsonDoc["NH3_concentration"] = nh3Concentration;
  jsonDoc["CO2_concentration"] = co2Concentration;

  // if (Serial.available()) {
  //   while (gpsSerial.available() > 0) {
  //     byte c = gpsSerial.read();
  //     Serial.write(c);
  //   }
  //   Serial.println();
  //   Serial.println();
  // }

  // Gửi dữ liệu JSON qua Serial
  serializeJson(jsonDoc, espSerial);
  serializeJson(jsonDoc,Serial);
  espSerial.println();
  Serial.println();

  delay(5000);
}
