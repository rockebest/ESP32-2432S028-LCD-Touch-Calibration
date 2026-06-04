#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("ESP32-2432S028 LCD color test start");

  tft.init();
  tft.setRotation(1);
}

void loop() {
  Serial.println("RED");
  tft.fillScreen(TFT_RED);
  delay(1000);

  Serial.println("GREEN");
  tft.fillScreen(TFT_GREEN);
  delay(1000);

  Serial.println("BLUE");
  tft.fillScreen(TFT_BLUE);
  delay(1000);

  Serial.println("WHITE");
  tft.fillScreen(TFT_WHITE);
  delay(1000);

  Serial.println("BLACK");
  tft.fillScreen(TFT_BLACK);
  delay(1000);
}
