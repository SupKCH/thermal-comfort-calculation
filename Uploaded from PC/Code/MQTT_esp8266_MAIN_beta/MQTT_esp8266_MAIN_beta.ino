#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Mitsubishi.h>
#include <Wire.h>


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
bool isUpdate[total_devices][2];              // 3 units; 2 data: 0 = Humid, 1 = Temp
unsigned long lastUpdate[total_devices][2];

const char* ssid = "chansak";
const char* password = "kornkarn";
//const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* serverName = "http://jtz.kyt.mybluehost.me/01_capstone_esp/main-esp-post-data.php";
const char* herokuServer = "http://thermal-comfort-calculation.herokuapp.com/";
String clientId = "ESP8266Client-";


String apiKeyValue = "tPmAT5Ab3j7F9";
String sensorName = "DHT22";
String sensorLocation = "KarnDesktop";

//unsigned long lastPOST = 0;
unsigned long timerDelay = 60 * 1000;

uint8_t count = 0;
uint8_t shot = 0;

byte command_t = 0;
float suggested_tdb = 0;;
float fit_MRT = 0;
float pmv = 0;
float ppd = 0;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long last_read = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

//StaticJsonDocument<192> doc;
//StaticJsonDocument<384> doc_in;
StaticJsonDocument<64> doc;
StaticJsonDocument<384> doc_in;
String output, input;

const uint16_t kIrLed = 10;  // ESP82466 GPIO pin to use. Recommended: 4 (D2).
//<<< Pin 10 works but need to declare as OUTPUT
IRMitsubishi136 ac(kIrLed);  // Set the GPIO used for sending messages.
long last_send = -600000;

bool have_slept = false;
byte old_command_t = 0;

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);
  pinMode(kIrLed, OUTPUT);
  ac.begin();
  Serial.begin(115200);
  Serial.println("Default state of the remote.");
  printState();
  Serial.println("Setting desired state for A/C.");
  ac.on();
  //ac.off()
  ac.setTemp(24);
  ac.setFan(3); //
  ac.setMode(kMitsubishi136Cool);
  /*
    case kMitsubishi136Fan:
    case kMitsubishi136Cool:
    //case kMitsubishi136Heat:
    case kMitsubishi136Auto:
    case kMitsubishi136Dry:
    default:
    _.Mode = kMitsubishi136Auto;
  */
  ac.setSwingV(kMitsubishi136SwingVHighest);
  /*
    case kMitsubishi136SwingVLowest:
    case kMitsubishi136SwingVLow:
    case kMitsubishi136SwingVHigh:
    case kMitsubishi136SwingVHighest:
    case kMitsubishi136SwingVAuto:
    default:
      _.SwingV = kMitsubishi136SwingVAuto;
  */

  dht.begin();

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  clientId += String(random(0xffff), HEX);
  //const int capacity = JSON_OBJECT_SIZE(3);

  //  doc["title"] = "Hello there";
  //  doc["tdb"] = 26.2;
  //  doc["tr"] = 30.7;
  //
  //  tod_list.add(30.2);
  //  tod_list.add(33.1);
  //  tod_list.add(32.8);
  //  doc["alpha"] = 0.9;
  //  doc["v"] = 0.3;
  //  serializeJsonPretty(doc, output);
  //  output = "";
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  if (!have_slept && millis() - last_read > timerDelay) {
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
      heroku_AC();
      //delay(200);
      POSTtoDatabase();
      delay(100);
    }
    have_slept = false;
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



void heroku_AC() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.useHTTP10(true);

    http.begin(espClient, herokuServer);
    http.addHeader("Content-Type", "application/json");

    doc["tdb"] = avg_temp;
    doc["rh"] = avg_humid;
    serializeJsonPretty(doc, output);
    int httpCode = http.POST(output);
    Serial.println(output);
    output = "";
    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_CREATED) {
        deserializeJson(doc_in, http.getStream());
        command_t = doc_in["selected_t"];
        suggested_tdb = doc_in["suggested_tdb"];
        fit_MRT = doc_in["fit_MRT"];
        pmv = doc_in["room_result"]["pmv"];
        ppd = doc_in["room_result"]["ppd"];
        if (command_t != old_command_t) {
          shot = 0;
        }
        Serial.print("Received temperature config: ");
        Serial.println(command_t);

        if (command_t != old_command_t && doc_in["tdb"] == avg_temp && doc_in["rh"] == avg_humid && millis() - last_send >= 1 * 60 * 1000 || shot < 3) {
          //#if SEND_MITSUBISHI_AC
          Serial.println("Sending IR command to A/C ...");
          ac.setTemp(command_t);
          ac.send();
          shot++;
          last_send = millis();
          //#endif
          printState();
          old_command_t = command_t;
        }

        serializeJsonPretty(doc_in, input);
        Serial.println("received payload:\n" + input);
        input = "";
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
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
                             + "&value5=" + String(command_t)
                             + "&value6=" + String(suggested_tdb)
                             + "&value7=" + String(fit_MRT)
                             + "&value8=" + String(pmv)
                             + "&value9=" + String(ppd)
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

void printState() {
  Serial.println("Mitsubishi A/C remote is in the following state:");
  Serial.printf("  %s\n", ac.toString().c_str());

  unsigned char* ir_code = ac.getRaw();
  Serial.print("IR Code: 0x");
  for (uint8_t i = 0; i < kMitsubishiACStateLength; i++)
    Serial.printf("%02X", ir_code[i]);
  Serial.println();
}
