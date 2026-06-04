# Troubleshooting

## White screen

A white screen usually means the backlight is on, but the LCD controller is not initialized correctly.

Check:

- `User_Setup.h` is actually being used by TFT_eSPI
- `User_Setup_Select.h` includes only `#include <User_Setup.h>`
- Display driver is not a TTGO T-Display or ST7789 135×240 preset
- Pins match the ESP32-2432S028 setup

Run this example from TFT_eSPI:

```text
File → Examples → TFT_eSPI → Test and diagnostics → Read_User_Setup
```

The output should show pins like:

```text
MOSI    = GPIO 13
SCK     = GPIO 14
TFT_CS  = GPIO 15
TFT_DC  = GPIO 2
TFT_BL  = GPIO 21
```

## Noise screen

A noisy screen means SPI signals are reaching the LCD, but the display driver or SPI speed may still be wrong.

Try:

```cpp
#define SPI_FREQUENCY 10000000
```

If it still shows noise, switch between:

```cpp
#define ILI9341_2_DRIVER
```

and:

```cpp
#define ILI9341_DRIVER
```

Only one display driver should be enabled at a time.

## Touch works but LCD does not

This is possible because the touch controller and LCD use different SPI pins.

Touch pins:

```cpp
#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25
#define XPT2046_CS   33
```

LCD pins:

```cpp
#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_BL   21
```

## Touch is offset or becomes worse toward a corner

Use affine calibration instead of simple `map()` calibration.

Simple mapping may be insufficient when the resistive touch panel is slightly skewed. The affine calibration sketch calculates:

```cpp
screenX = ax * rawX + bx * rawY + cx;
screenY = ay * rawX + by * rawY + cy;
```

## Recommended rotation

The included examples use:

```cpp
tft.setRotation(1);
ts.setRotation(0);
```

Changing rotation requires recalibration.
