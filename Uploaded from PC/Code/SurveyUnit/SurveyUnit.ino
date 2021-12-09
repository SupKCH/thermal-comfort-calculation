/*
   IRremoteESP8266: IRrecvDemo - demonstrates receiving IR codes with IRrecv
   This is very simple teaching code to show you how to use the library.
   If you are trying to decode your Infra-Red remote(s) for later replay,
   use the IRrecvDumpV2.ino (or later) example code instead of this.
   An IR detector/demodulator must be connected to the input kRecvPin.
   Copyright 2009 Ken Shirriff, http://arcfn.com
   Example circuit diagram:
    https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-receiving
   Changes:
     Version 0.2 June, 2017
       Changed GPIO pin to the same as other examples.
       Used our own method for printing a uint64_t.
       Changed the baud rate to 115200.
     Version 0.1 Sept, 2015
       Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009
*/

#include <Arduino.h>
#include <assert.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 10
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
unsigned long last_read = 0;

const uint16_t kRecvPin = 14;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
#if DECODE_AC
const uint8_t kTimeout = 50;
#else
const uint8_t kTimeout = 15;
#endif
const uint16_t kMinUnknownSize = 12;
const uint8_t kTolerancePercentage = kTolerance;
#define LEGACY_TIMING_INFO false
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;

void setup() {
#if defined(ESP8266)
  Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(kBaudRate, SERIAL_8N1);
#endif  // ESP8266
  assert(irutils::lowLevelSanityCheck() == 0);
  Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", kRecvPin);
#if DECODE_HASH
  irrecv.setUnknownThreshold(kMinUnknownSize);
#endif  // DECODE_HASH
  irrecv.setTolerance(kTolerancePercentage);  // Override the default tolerance.
  irrecv.enableIRIn();  // Start the receiver
  dht.begin();
  irrecv.enableIRIn();  // Start the receiver

  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  //  display.println("Welcome All to");
  //  display.setTextSize(2);
  //  display.println("ArduinoAll");
  //  display.setTextColor(BLACK, WHITE);
  //  display.setTextSize(1);
  //  display.println("www.ArduinoAll.com");
}

void loop() {
  if (millis() - last_read > 2000) {
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%\t  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));
    
    display.clearDisplay();
    display.setCursor(0, 0);

    display.setTextSize(1);
    display.println(F("Humidity: "));
    display.setTextSize(2);
    display.print(h);
    display.println(F(" %"));

    display.setTextSize(1);
    display.println("");
    display.println(F("Temperature: "));
    display.setTextSize(2);
    display.print(t);
    display.println(F(" C "));
    display.display();
    last_read = millis();
  }

  if (irrecv.decode(&results)) {
    uint32_t now = millis();
    Serial.printf(D_STR_TIMESTAMP " : %06u.%03u\n", now / 1000, now % 1000);
    if (results.overflow)
      Serial.printf(D_WARN_BUFFERFULL "\n", kCaptureBufferSize);

    Serial.println(D_STR_LIBRARY "   : v" _IRREMOTEESP8266_VERSION_ "\n");

    if (kTolerancePercentage != kTolerance)
      Serial.printf(D_STR_TOLERANCE " : %d%%\n", kTolerancePercentage);

    Serial.print(resultToHumanReadableBasic(&results));

    String description = IRAcUtils::resultAcToString(&results);
    if (description.length()) Serial.println(D_STR_MESGDESC ": " + description);
    yield();
#if LEGACY_TIMING_INFO
    Serial.println(resultToTimingInfo(&results));
    yield();
#endif
    Serial.println(resultToSourceCode(&results));
    Serial.println();    // Blank line between entries
    yield();             // Feed the WDT (again)
  }
  delay(100);
}
