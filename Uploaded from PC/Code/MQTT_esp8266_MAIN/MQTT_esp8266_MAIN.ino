#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define DHTPIN 14
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);
float humid, temp, hIndex;
bool newData = false;
char buff[50];
char p_humid[10], p_temp[10], p_hIndex[10];
const uint8_t total_devices = 3;
float rec_humid[total_devices], rec_temp[total_devices];
float avg_humid, avg_temp, heat_index;
bool isUpdate[total_devices][2];              // 3 units, 2 data; 0 = Humid, 1 = Temp
unsigned long lastUpdate[total_devices][2];

const char* ssid = "chansak";
const char* password = "kornkarn";
//const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* serverName = "http://jtz.kyt.mybluehost.me/01_capstone_esp/main-esp-post-data.php";
String clientId = "ESP8266Client-";


String apiKeyValue = "tPmAT5Ab3j7F9";
String sensorName = "DHT22";
String sensorLocation = "KarnDesktop";

//unsigned long lastPOST = 0;
unsigned long timerDelay = 10000;

uint8_t count = 0;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long last_read = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  clientId += String(random(0xffff), HEX);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  if (millis() - last_read > timerDelay) {
    rec_humid[0] = dht.readHumidity();
    rec_temp[0] = dht.readTemperature();
    if (!isnan(rec_humid[0]) && !isnan(rec_temp[0])) {
      newData = true;
    }
    else {
      //client.publish("errorLog", "Unit 0: Failed to read from DHT sensor!");
      delay(1000);
      Serial.println(F("Failed to read from DHT sensor!"));
    }

    if (newData) {
      avg_humid = average(rec_humid);
      avg_temp = average(rec_temp);
      heat_index = dht.computeHeatIndex(avg_temp, avg_humid, false);
      Serial.print(avg_humid, 2);
      Serial.print("\t");
      Serial.print(avg_temp, 2);
      Serial.print("\t");
      Serial.println(heat_index);
      clearDeadData('h');
      clearDeadData('t');
      newData = false;
      last_read = millis();
      POSTtoDatabase();
      delay(100);
    }
  }
}

//////////////////////////////////////////////////////////////////
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int Length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String string_payload = "";
  for (int i = 0; i < Length; i++) {
    string_payload += (char)payload[i];
  }
  Serial.print(string_payload);
  char char_payload[10];
  string_payload.toCharArray(char_payload, 10);
  Serial.print(" --> ");
  float float_payload = atof(char_payload);
  Serial.println(float_payload);
  switch (topic[5]) {
    case '1':
      switch (topic[7]) {
        case 'H':
          rec_humid[1] = float_payload;
          isUpdate[1][0] = true;
          lastUpdate[1][0] = millis();
          break;
        case 'T':
          rec_temp[1] = float_payload;
          isUpdate[1][1] = true;
          lastUpdate[1][1] = millis();
          break;
        default:
          Serial.println("1: Subtopic not matched");
          break;
      }
      break;
    case '2':
      switch (topic[7]) {
        case 'H':
          rec_humid[2] = float_payload;
          isUpdate[2][0] = true;
          lastUpdate[2][0] = millis();
          break;
        case 'T':
          rec_temp[2] = float_payload;
          isUpdate[2][1] = true;
          lastUpdate[2][1] = millis();
          break;
        default:
          Serial.println("2: Subtopic not matched");
          break;
      }
      break;
  }
}

void reconnect() {
  while (!client.connected()) {
    //digitalWrite(BUILTIN_LED, LOW);
    Serial.print("Attempting MQTT connection...");

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

      client.subscribe("Unit 1/Humidity");
      client.subscribe("Unit 1/Temperature");

      client.subscribe("Unit 2/Humidity");
      client.subscribe("Unit 2/Temperature");

      //digitalWrite(BUILTIN_LED, HIGH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");

      delay(3000);
    }
  }
}

float average(float* rec_data) {
  count = 0;
  float avg = 0;
  float total_buff = 0;
  for (uint8_t i = 0; i < total_devices; i++) {
    if (rec_data[i] > 0) {
      total_buff += rec_data[i];
      count++;
    }
  }
  if (count > 0) {
    avg = total_buff / count;
  }
  else {
    avg = 0;
  }
  return avg;
}

void clearDeadData(char data_mode) {
  if (data_mode == 'h' && (isUpdate[1][0] || isUpdate[2][0])) {
    for (uint8_t i = 1; i < total_devices; i++) {
      if (millis() - lastUpdate[i][0] > 25000) {
        rec_humid[i] = 0;
        isUpdate[i][0] = false;
      }
    }
  }
  else if (data_mode == 't' && (isUpdate[1][1] || isUpdate[2][1])) {
    for (uint8_t i = 1; i < total_devices; i++) {
      if (millis() - lastUpdate[i][1] > 25000) {
        rec_temp[i] = 0;
        isUpdate[i][1] = false;
      }
    }
  }
}

void POSTtoDatabase() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(espClient, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "api_key=" + apiKeyValue
                             + "&value1=" + String(count)
                             + "&value2=" + String(avg_humid)
                             + "&value3=" + String(avg_temp)
                             + "&value4=" + String(heat_index)
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
  //lastPOST = millis();
}

/*
  sprintf(p_humid, "%.1f", rec_humid[0]);
  sprintf(p_temp, "%.1f", rec_temp[0]);
  //sprintf(p_hIndex, "%.2f", hIndex);
  client.publish("Unit 0/Humidity", p_humid);
  client.publish("Unit 0/Temperature", p_temp);
  //client.publish("Unit 0/HeatIndex", p_hIndex);
  sprintf(buff, "Publish message: %.1f, %.1f", humid, temp);
  Serial.println(buff);
  lastMsg = millis();
  newData = false;
*/
