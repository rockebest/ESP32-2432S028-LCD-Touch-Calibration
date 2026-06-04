// User_Setup_ESP32_2432S028.h
// TFT_eSPI setup for ESP32-2432S028 style 2.8-inch TFT LCD board.
// Copy this file content into TFT_eSPI/User_Setup.h.

#define USER_SETUP_INFO "ESP32-2432S028"

// For many ESP32-2432S028 boards, ILI9341_2 works better than ILI9341.
// If you still see noise or a white screen, try ILI9341_DRIVER instead.
#define ILI9341_2_DRIVER
// #define ILI9341_DRIVER
// #define ST7789_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1

#define TFT_BL   21
#define TFT_BACKLIGHT_ON HIGH

#define USE_HSPI_PORT

// Start with a conservative SPI frequency for stability.
// After the display works, you can try 27000000 or 40000000.
#define SPI_FREQUENCY       10000000
#define SPI_READ_FREQUENCY   8000000
#define SPI_TOUCH_FREQUENCY  2500000

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT
