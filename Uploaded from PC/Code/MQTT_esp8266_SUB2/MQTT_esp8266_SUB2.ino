#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);
float humid, temp, hIndex;
bool newData = false;
char buff[50];
char p_humid[10], p_temp[10], p_hIndex[10];

// Update these with values suitable for your network.
const char* ssid = "chansak";
const char* password = "kornkarn";
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //delay(1000);

  if (millis() - lastMsg > 4000) {
    humid = dht.readHumidity();
    temp = dht.readTemperature();
    if (isnan(humid) || isnan(temp)) {
      client.publish("errorLog", "unit 1: Failed to read from DHT sensor!");
      delay(1000);
      //Serial.println(F("Failed to read from DHT sensor!"));
    }
    else {
      newData = true;
      hIndex = dht.computeHeatIndex(temp, humid, false);
    }

    if (newData) {
      sprintf(p_humid, "%.1f", humid);
      sprintf(p_temp, "%.1f", temp);
      sprintf(p_hIndex, "%.2f", hIndex);
      client.publish("Unit 2/Humidity", p_humid);
      client.publish("Unit 2/Temperature", p_temp);
      //client.publish("Unit 2/HeatIndex", p_hIndex);
      sprintf(buff, "Publish message: %.1f, %.1f", humid, temp);
      Serial.println(buff);
      lastMsg = millis();
      newData = false;
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
  for (int i = 0; i < Length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  /*
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
  }
  */
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    digitalWrite(BUILTIN_LED, LOW);
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");    // << --- set subscription here
      digitalWrite(BUILTIN_LED, HIGH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
