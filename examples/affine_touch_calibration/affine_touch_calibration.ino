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

struct Point {
  float x;
  float y;
};

Point rawTL, rawTR, rawBR, rawBL;

float ax, bx, cx;
float ay, by, cy;

void drawCross(int x, int y, uint16_t color) {
  tft.drawLine(x - 12, y, x + 12, y, color);
  tft.drawLine(x, y - 12, x, y + 12, color);
  tft.drawCircle(x, y, 9, color);
}

Point readTouchAverage() {
  Point avg = {0, 0};
  int count = 0;

  while (!ts.touched()) {
    delay(10);
  }

  Serial.println("Touch detected. Keep holding...");

  unsigned long startTime = millis();

  while (count < 25 && millis() - startTime < 3000) {
    if (ts.touched()) {
      TS_Point p = ts.getPoint();

      // Ignore invalid samples such as 0,0.
      if (p.x > 50 && p.y > 50 && p.z > 50) {
        avg.x += p.x;
        avg.y += p.y;
        count++;
        Serial.print(".");
      }
    }

    delay(30);
  }

  Serial.println();

  if (count == 0) {
    Serial.println("No valid touch samples. Try again.");
    avg.x = 0;
    avg.y = 0;
    return avg;
  }

  avg.x /= count;
  avg.y /= count;

  Serial.print("Valid samples = ");
  Serial.println(count);
  Serial.print("raw x=");
  Serial.print(avg.x);
  Serial.print(", raw y=");
  Serial.println(avg.y);

  while (ts.touched()) {
    delay(10);
  }

  delay(300);

  return avg;
}

Point getCalibrationPoint(const char* label, int sx, int sy) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(label, 30, 20, 2);
  tft.drawString("Touch the cross center", 30, 45, 2);

  drawCross(sx, sy, TFT_YELLOW);

  Serial.println(label);
  return readTouchAverage();
}

void computeAffine(Point r1, Point r2, Point r3,
                   Point s1, Point s2, Point s3) {
  float det = r1.x * (r2.y - r3.y)
            - r2.x * (r1.y - r3.y)
            + r3.x * (r1.y - r2.y);

  if (abs(det) < 0.001) {
    Serial.println("Calibration failed: det too small");
    Serial.println("Please run calibration again and keep holding each cross for about 1 second.");
    return;
  }

  ax = (s1.x * (r2.y - r3.y)
      - s2.x * (r1.y - r3.y)
      + s3.x * (r1.y - r2.y)) / det;

  bx = (r1.x * (s2.x - s3.x)
      - r2.x * (s1.x - s3.x)
      + r3.x * (s1.x - s2.x)) / det;

  cx = (r1.x * (r2.y * s3.x - r3.y * s2.x)
      - r2.x * (r1.y * s3.x - r3.y * s1.x)
      + r3.x * (r1.y * s2.x - r2.y * s1.x)) / det;

  ay = (s1.y * (r2.y - r3.y)
      - s2.y * (r1.y - r3.y)
      + s3.y * (r1.y - r2.y)) / det;

  by = (r1.x * (s2.y - s3.y)
      - r2.x * (s1.y - s3.y)
      + r3.x * (s1.y - s2.y)) / det;

  cy = (r1.x * (r2.y * s3.y - r3.y * s2.y)
      - r2.x * (r1.y * s3.y - r3.y * s1.y)
      + r3.x * (r1.y * s2.y - r2.y * s1.y)) / det;

  Serial.println("===== Affine Calibration Result =====");
  Serial.println("Copy these values into calibrated_touch_test.ino or your own sketch.");
  Serial.print("const float ax = "); Serial.print(ax, 8); Serial.println(";");
  Serial.print("const float bx = "); Serial.print(bx, 8); Serial.println(";");
  Serial.print("const float cx = "); Serial.print(cx, 8); Serial.println(";");
  Serial.println();
  Serial.print("const float ay = "); Serial.print(ay, 8); Serial.println(";");
  Serial.print("const float by = "); Serial.print(by, 8); Serial.println(";");
  Serial.print("const float cy = "); Serial.print(cy, 8); Serial.println(";");
  Serial.println("=====================================");
}

bool getTouchAffine(int &sx, int &sy) {
  if (!ts.touched()) {
    return false;
  }

  TS_Point p = ts.getPoint();

  float x = ax * p.x + bx * p.y + cx;
  float y = ay * p.x + by * p.y + cy;

  sx = constrain((int)x, 0, SCREEN_W - 1);
  sy = constrain((int)y, 0, SCREEN_H - 1);

  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(touchSPI);
  ts.setRotation(0);

  Serial.println("Affine touch calibration start");

  // Use points slightly inside the screen edges for better accuracy.
  rawTL = getCalibrationPoint("1/4 Top Left",      30,  30);
  rawTR = getCalibrationPoint("2/4 Top Right",    290,  30);
  rawBR = getCalibrationPoint("3/4 Bottom Right", 290, 210);
  rawBL = getCalibrationPoint("4/4 Bottom Left",   30, 210);

  // Use TL, TR, and BL for affine calculation.
  computeAffine(
    rawTL, rawTR, rawBL,
    {30, 30}, {290, 30}, {30, 210}
  );

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Affine touch test", 50, 20, 2);
  tft.drawString("Touch anywhere", 50, 45, 2);
}

void loop() {
  int x, y;

  if (getTouchAffine(x, y)) {
    Serial.print("screen x=");
    Serial.print(x);
    Serial.print(", y=");
    Serial.println(y);

    tft.fillCircle(x, y, 4, TFT_RED);
    delay(80);
  }
}
