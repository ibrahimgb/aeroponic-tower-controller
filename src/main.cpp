#include <Arduino.h>
#include <WiFi.h>
#include <stdlib.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ClosedCube_HDC1080.h"
#include <string.h>


#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Tenda_423FF0";
const char* password = "JBM120756";

int timeOn = 15;
int timeOff = 25;

unsigned long lastTimeRedjesterd = millis();
unsigned long Delay = 500;

char jsonOutput[512];

void initWiFi() {
  WiFi.mode(WIFI_STA);
  //WiFi.begin(getenv("ssid") , getenv("password") );
  WiFi.begin(ssid, password);
  //WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println("wifi connected");
}

void sendData(){
  if(WiFi.status() == WL_CONNECTED){

  HTTPClient client;

  client.begin("http://192.168.1.14:3000/sensor/newReading");
  client.addHeader("Content-Type", "application/json");

  //const size_t CAPACITY = JSON_OBJECT_SIZE(1);
  DynamicJsonDocument doc(512);

  JsonObject root = doc.to<JsonObject>();
  root["AeroponicTowerId"] = "id";
  JsonObject waterTemperature = root.createNestedObject("waterTemperature");
  waterTemperature["fahrenheit"] = 14.2;
  waterTemperature["celsius"] = 33.63;
  JsonObject envTempAndHumidity = root.createNestedObject("envTempAndHumidity");
  envTempAndHumidity["fahrenheit"]= 22;
  envTempAndHumidity["celsius"]= 34;
  envTempAndHumidity["humidity"]= 60.5;
  JsonObject insideTempAndHumidity = root.createNestedObject("insideTempAndHumidity");
  insideTempAndHumidity["fahrenheit"]= 22;
  insideTempAndHumidity["celsius"]= 34;
  insideTempAndHumidity["humidity"]= 60.5;
  root["uvLight"]= 2.2;
  root["waterNeedsRefilling"]= false;
  root["pumpIsWorking"]= true;

  serializeJsonPretty(root, jsonOutput);
  Serial.println(jsonOutput);



  int httpCode = client.POST(String(jsonOutput));
  if (httpCode > 0 ){
    String payload = client.getString();
    Serial.println("status code" + String(httpCode));
    Serial.println(payload);

    // char json[500];
    // payload.replace(" ", "");
    // payload.replace("\n", "");
    // payload.trim();
    // payload.remove(0,1);
    // payload.toCharArray(json,500);

    StaticJsonDocument<200> doc;
    deserializeJson(doc,payload);
    if( doc["timeOn"] > 0 && doc["timeOn"] < 60){
      timeOn = doc["timeOn"];
      Serial.println("time On have been updated to : "+ timeOn);
    }

    if( doc["timeOff"] > 0 && doc["timeOff"] < 120){
      timeOff = doc["timeOff"];
      Serial.println("time off have been updated to : "+ timeOff);
    }

    
    

    Serial.println(String(timeOff) + " - " + String(timeOn) + "\n");

    client.end();



  }else{
    Serial.println("errer on req errer code:" + String(httpCode));
  }
}

}



void setup() {
   Serial.begin(9600);
  initWiFi();
 
}

void loop() {


  unsigned long timeNow = millis();
  if(timeNow - lastTimeRedjesterd > Delay){
    sendData();
    lastTimeRedjesterd = millis();
  }


  if(WiFi.status() != WL_CONNECTED){
    initWiFi();
  }


  delay(5000);
}