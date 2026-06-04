#include <TFT_eSPI.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>

#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25
#define XPT2046_CS   33

TFT_eSPI tft = TFT_eSPI();

SPIClass touchSPI = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

const int SCREEN_W = 320;
const int SCREEN_H = 240;

// Example calibration values from one ESP32-2432S028 board.
// Replace these with your own values from affine_touch_calibration.ino.
const float ax = 0.00003874;
const float bx = 0.08726453;
const float cx = -13.03474426;

const float ay = -0.06659155;
const float by = 0.00032632;
const float cy = 256.75961304;

bool getTouch(int &x, int &y) {
  if (!ts.touched()) {
    return false;
  }

  TS_Point p = ts.getPoint();

  float fx = ax * p.x + bx * p.y + cx;
  float fy = ay * p.x + by * p.y + cy;

  x = constrain((int)fx, 0, SCREEN_W - 1);
  y = constrain((int)fy, 0, SCREEN_H - 1);

  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Calibrated Touch Test", 35, 20, 2);
  tft.drawString("Touch anywhere", 80, 50, 2);

  touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(touchSPI);
  ts.setRotation(0);

  Serial.println("Calibrated touch test start");
}

void loop() {
  int x, y;

  if (getTouch(x, y)) {
    Serial.print("x = ");
    Serial.print(x);
    Serial.print(", y = ");
    Serial.println(y);

    tft.fillCircle(x, y, 4, TFT_RED);
    delay(80);
  }
}
