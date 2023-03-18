#include <Arduino.h>
#include <WiFi.h>
#include <stdlib.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ClosedCube_HDC1080.h"
#include <string.h>
#include "time.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Tenda_423FF0";
const char* password = "JBM120756";



unsigned long lastTimeRegistered = millis();
unsigned long Delay = 5000;

unsigned long lastTimeRelayRegistered = millis();
unsigned long timeOn = 3000;
unsigned long timeOff = 2000;
boolean relayStateIsOn = false; 

int relay = 26;

char jsonOutput[512];



// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";

// Variable to save current epoch time
unsigned long epochTime; 

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

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
  root["waterTemperature"] = 22;
  JsonObject envTempAndHumidity = root.createNestedObject("envTempAndHumidity");
  envTempAndHumidity["temperature"]= 22;
  envTempAndHumidity["humidity"]= 60.5;
  JsonObject insideTempAndHumidity = root.createNestedObject("insideTempAndHumidity");
  insideTempAndHumidity["temperature"]= 22;
  insideTempAndHumidity["humidity"]= 60.5;
  root["uvLight"]= 2.2;
  root["waterNeedsRefilling"]= false;
  root["pumpIsWorking"]= true;
  root["epochTime"]= getTime();

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


void checkRelay(long timeNow){

if(timeNow - lastTimeRelayRegistered > timeOn && relayStateIsOn){
    Serial.println("relay turned off");
    relayStateIsOn = !relayStateIsOn;
    digitalWrite(relay, LOW);
    lastTimeRelayRegistered = millis();
  }
  if(timeNow - lastTimeRelayRegistered > timeOff && !relayStateIsOn){
    Serial.println("relay turned on");
    relayStateIsOn = !relayStateIsOn;
    digitalWrite(relay, HIGH);
    lastTimeRelayRegistered = millis();
  }

}



void setup() {
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  initWiFi();
  configTime(0, 0, ntpServer);
}

void loop() {


  unsigned long timeNow = millis();
  if(timeNow - lastTimeRegistered > Delay){
    sendData();
    lastTimeRegistered = millis();
  }


  //checkRelay(timeNow);

  if(WiFi.status() != WL_CONNECTED){
    initWiFi();
  }

}
