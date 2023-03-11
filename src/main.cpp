#include <Arduino.h>
#include <WiFi.h>
#include <stdlib.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ClosedCube_HDC1080.h"

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(getenv("ssid") , getenv("password") );
  //WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

// For Temperature Water Sensor
// GPIO where the DS18B20 is connected to
const int oneWireBus = 12;     
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

 
ClosedCube_HDC1080 hdc1080;



void readTemperatureWaterSensor(){
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");
  
}


void readTemperatureEnvironmentSensor(){

Serial.print("T=");
Serial.print(hdc1080.readTemperature());
Serial.print("C, RH=");
Serial.print(hdc1080.readHumidity());
Serial.println("%");

}


void setup() {
   Serial.begin(9600);

  sensors.begin();


  hdc1080.begin(0x40);
  Serial.print("Manufacturer ID=0x");
Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
Serial.print("Device ID=0x");
Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device
 
}

void loop() {
  readTemperatureWaterSensor();
  readTemperatureEnvironmentSensor();


  delay(5000);
}