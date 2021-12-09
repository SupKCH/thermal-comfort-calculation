#include <ESP8266WiFi.h>
#include "DHT.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
float humid, temp, hIndex, light;
bool newData = false;

const char* ssid = "chansak";
const char* password = "kornkarn";
const char* serverName = "http://jtz.kyt.mybluehost.me/01_capstone_esp/nextTo-esp-post-data.php";
String apiKeyValue = "tPmAT5Ab3j7F9";

WiFiClient espClient;
unsigned long lastMsg = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
}

void loop() {
  if (millis() - lastMsg > 10000) {
    humid = dht.readHumidity();
    temp = dht.readTemperature();
    if (!isnan(humid) && !isnan(temp)) {
      newData = true;
      hIndex = dht.computeHeatIndex(temp, humid, false);
    }
    else {
      delay(1000);
      //Serial.println(F("Failed to read from DHT sensor!"));
    }

    if (newData) {
      lastMsg = millis();
      newData = false;
      POSTtoDatabase2(apiKeyValue, humid, temp, hIndex);
    }
  }
}

//////////////////////////////////////////////////////////////////
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void POSTtoDatabase2(String api, float value_1, float value_2, float value_3) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(espClient, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "api_key=" + api
                             + "&value1=" + String(value_1)
                             + "&value2=" + String(value_2)
                             + "&value3=" + String(value_3)
                             + "";
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
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}
