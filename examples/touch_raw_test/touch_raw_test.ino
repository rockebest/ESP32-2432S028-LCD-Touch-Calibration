#include <SPI.h>
#include <XPT2046_Touchscreen.h>

#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25
#define XPT2046_CS   33

SPIClass touchSPI = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

void setup() {
  Serial.begin(115200);
  delay(1000);

  touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(touchSPI);
  ts.setRotation(0);

  Serial.println("ESP32-2432S028 raw touch test start");
  Serial.println("Touch the screen and check raw x, y, z values.");
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();

    Serial.print("raw x = ");
    Serial.print(p.x);
    Serial.print(", raw y = ");
    Serial.print(p.y);
    Serial.print(", z = ");
    Serial.println(p.z);
  }

  delay(100);
}
