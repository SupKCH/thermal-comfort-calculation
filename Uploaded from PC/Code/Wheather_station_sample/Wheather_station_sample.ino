/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/cloud-weather-station-esp32-esp8266/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

*/

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#endif

#include <Wire.h>
#include "DHT.h"

#define DHTPIN 3
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "chansak";
const char* password = "kornkarn";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://jtz.kyt.mybluehost.me/esp-post-data.php";

String apiKeyValue = "tPmAT5Ab3j7F9";
String sensorName = "DHT22";
String sensorLocation = "KarnDesktop";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      /*String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
        + "&location=" + sensorLocation + "&value1=" + String(dht.readTemperature())
        + "&value2=" + String(dht.readHumidity()) + "&value3=" + String(0) + "";*/

      String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";
      Serial.print("httpRequestData: ");
      Serial.println(httpRequestData);

      int httpResponseCode = http.POST(httpRequestData);

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

// If you need an HTTP request with a content type: text/plain
//http.addHeader("Content-Type", "text/plain");
//int httpResponseCode = http.POST("Hello, World!");

// If you need an HTTP request with a content type: application/json, use the following:
//http.addHeader("Content-Type", "application/json");
//int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
