#include <SoftwareSerial.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <MQ135.h>

#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int MQ7_PIN = A0;
const int MQ135_PIN = A4;
MQ135 gasSensor = MQ135(MQ135_PIN);
SoftwareSerial espSerial(10, 11);
StaticJsonDocument<200> jsonDoc;

//gp2y10
int measurePin = A5;
int ledPower = 8;

unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  espSerial.begin(4800);
  pinMode(ledPower,OUTPUT);
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  float rs = gasSensor.getCorrectedResistance(temperature, humidity);
  int mq7Value = analogRead(MQ7_PIN); 
  float coConcentration = map(mq7Value, 0, 1023, 0, 100);

  int mq135Value = analogRead(MQ135_PIN);
  float nh3Concentration = 102.2*pow(rs/76.63, -2.473);
  float co2Concentration = 110.47*pow(rs/76.63, -2.862);
  
  jsonDoc["humidity"] = humidity;
  jsonDoc["temperature"] = temperature;
  jsonDoc["CO_concentration"] = coConcentration;
  jsonDoc["NH3_concentration"] = nh3Concentration;
  jsonDoc["CO2_concentration"] = co2Concentration;

  //PM2.5
    digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin);

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH);
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured*(5.0/1024);
  dustDensity = 0.17*calcVoltage-0.1;

  if ( dustDensity < 0)
  {
    dustDensity = 0.00;
  }
  jsonDoc["PM25_concentration"] = dustDensity;
  Serial.println(humidity);

  // Gửi dữ liệu JSON qua Serial
  serializeJson(jsonDoc, espSerial);
  serializeJson(jsonDoc,Serial);
  espSerial.println();
  Serial.println();

  delay(5000);
}
