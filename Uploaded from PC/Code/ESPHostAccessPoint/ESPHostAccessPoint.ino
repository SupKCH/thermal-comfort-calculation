/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
#include <DHT.h>

#define DHTPIN 10 // checked, workable in DHTtester
#define DHTTYPE DHT22

#ifndef APSSID
#define APSSID "ESP-Iot"
#define APPSK  "AC_Control"
#endif

DHT dht(DHTPIN, DHTTYPE);

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

const byte clientLimit = 4;
float humidity[clientLimit];
float temperature[clientLimit];
float heatIndex[clientLimit];
char output[50];
char avg[50];
byte onlineClient;

WiFiServer server(80);
/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //server.on("/", handleRoot);
  server.begin();
  //Serial.println("HTTP server started");
}

void loop() {
  onlineClient = WiFi.softAPgetStationNum();
  WiFiClient client = server.available();
  if (client.connected()) {
    String in_message = client.readStringUntil('\r');    // reads the message from the client
    char char_message[in_message.length()];
    in_message.toCharArray(char_message, in_message.length());
    //Serial.println(in_message);

    IPAddress clientIP = client.remoteIP();
    byte slot = clientIP[3] - 2;
    //Serial.println(clientIP);
    humidity[slot] = atof(strtok((char *)char_message, ","));  // Look for a comma, the return the data before it.
    temperature[slot] = atof(strtok(NULL, ","));
    heatIndex[slot] = dht.computeHeatIndex(temperature[slot], humidity[slot], false);
    //heatIndex[slot] = atof(strtok(NULL, ","));
    for (byte i = 0;  i < onlineClient ; i++) {
      //sprintf(output, "<%d> HUMID: %.1f %RH, TEMP: %.1f °C, HeatIndex: %.2f °C", i, humidity[i], temperature[i], heatIndex[i]);
      sprintf(output, "<%d> HUMID: %.1f %RH, TEMP: %.1f °C", i, humidity[i], temperature[i]);
      Serial.print(output);
      Serial.print(" | ");
    }
    float avg_humid = average(humidity, onlineClient);
    float avg_temp = average(temperature, onlineClient);
    float avg_heatIndex = dht.computeHeatIndex(avg_temp, avg_humid, false);
    sprintf(avg, "<AVG> HUMID: %.1f %RH, TEMP: %.1f °C, HeatIndex: %.2f °C", avg_humid, avg_temp, avg_heatIndex);
    Serial.println(avg);
    client.flush();
  }
  client.stop();                         // disconnects the client

  delay(100);
}

float average(float *data, byte len) {
  float sum = 0 ;
  for (int i = 0 ; i < len ; i++)
    sum += data [i] ;
  return  (sum / len) ;
}

//void handleRoot() {
//  server.send(200, "text/html", "<h1>You are connected</h1>");
//}
