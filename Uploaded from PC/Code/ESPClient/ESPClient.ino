/*
    This sketch establishes a TCP connection to a "quote of the day" service.
    It sends a "hello" message, and then prints received data.
*/

#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN 4     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!

#ifndef STASSID
#define STASSID "ESP-Iot"
#define STAPSK  "AC_Control"
#endif
#define LED_builtin 16

DHT dht(DHTPIN, DHTTYPE);

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.4.1";
const uint16_t port = 80;

float h, t, hic;
char message[20];
bool newData = false;

void setup() {
  //pinMode(LED_builtin, OUTPUT);
  //digitalWrite(LED_builtin, HIGH);
  Serial.begin(115200);
  dht.begin();

  // We start by connecting to a WiFi network
  /* Explicitly set the ESP8266 to be a WiFi-client */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress myClientIP = WiFi.localIP();
  Serial.println(myClientIP);
  //digitalWrite(LED_builtin, LOW);
}

void loop() {
  // This will send a string to the server
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(t, h, false);
  delay(400);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(2000);
  }
  else {
    newData = true;
    Serial.println("Successful reading DHT22");
  }

  // Use WiFiClient class to create TCP connections

  WiFiClient client;
  //  if (!client.connect(host, port)) {
  //    Serial.println("Connection failed");
  //    delay(5000);
  //    return;
  //  }

  if (newData) {
    Serial.print("Connecting to ");
    Serial.print(host);
    Serial.print(':');
    Serial.println(port);
    if (client.connect(host, port)) {
      sprintf(message, "%f,%f,%f,", h, t, hic);
      client.println(message);
      Serial.println(message);
      newData = false;
    }
  }
  /*
    // wait for data to be available
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        delay(60000);
        return;
      }
    }
    if (client.available() != 0) {
      Serial.println("Receiving from remote server");
      while (client.available()) {
        char ch = static_cast<char>(client.read());
        Serial.print("Server reply: ");
        Serial.println(ch);
      }
    }
  */
  // Close the connection
  Serial.println(".");
  Serial.println("Closing connection");
  client.stop();
  Serial.println("Connection closed");
  Serial.println("--------------------------------");
}
