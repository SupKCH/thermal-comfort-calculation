#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
#include <movingAvg.h>

movingAvg mov_avg(5);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float buff[10];
long count = 0;

void setup() {
  Serial.begin(9600);
  mov_avg.begin();
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay(); // ลบภาพในหน้าจอทั้งหมด
  display.setTextColor(WHITE, BLACK);  //กำหนดข้อความสีขาว ฉากหลังสีดำ
  display.setCursor(0, 0); // กำหนดตำแหน่ง x,y ที่จะแสดงผล
  display.setTextSize(2); // กำหนดขนาดตัวอักษร
  display.println("OLED"); // แสดงผลข้อความ ALL
  //
  //  display.setTextSize(1);
  //  display.print("welcome to");
  //  display.println(" All");
  //  display.setTextColor(BLACK, WHITE); //กลับสีข้อความกับพื้นหลัง
  //  display.print("www.arduinoall.com");
  //  display.setTextColor(WHITE, BLACK);
  //  display.setCursor(60, 0);
  //  display.setTextSize(2);
  //
  //  display.setCursor(0, 40);
  //  display.setTextSize(2);
  //  display.println("ArduinoALL"); // แสดงผลข้อความ ALL

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextSize(WHITE, BLACK);
  display.setCursor(0, 0);
  display.display();
  delay(100);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  // Invert and restore display, pausing in-between
  //  display.invertDisplay(true);
  //  delay(1000);
  //  display.invertDisplay(false);
  //  delay(1000);

}

void loop() {

  float amb_read = mlx.readAmbientTempC();
  float obj_read = mlx.readObjectTempC();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Emissivity: "); display.println(mlx.readEmissivity());
  display.setTextSize(1);
  display.print("Ambient = ");
  display.setTextSize(1);
  display.print(amb_read); display.println("*C");

  display.setTextSize(1);
  display.println("Object = ");
  display.setTextSize(2);
  display.print(obj_read); display.println(" *C");
  
  display.setTextSize(1);
  display.println("AVG = ");
  display.setTextSize(2);
  display.print(mov_avg.reading(obj_read*100)/100.); display.println(" *C");

  display.display();
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println(" *C");
  delay(500);
  display.clearDisplay();

}
