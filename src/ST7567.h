// Make a header guard.
#ifndef __ST7567_LCD_H__
#define __ST7567_LCD_H__

// Add main Arduino Header file.
#include <Arduino.h>

// Add Arduino SPI libarry for communication with the LCD.
#include <SPI.h>

// Include Adafruit GFX (bundled wih this library). Thanks Adafruit!
#include "Adafruit_GFX/Adafruit_GFX.h"

// Define LCD width and height in pixels.
#define ST7567_LCD_WIDTH    128
#define ST7567_LCD_HEIGHT   64

// LCD Settings and registers
#define ST7567_LCD_CMD_BIAS_SELECT      0b10100010
#define ST7567_LCD_CMD_SEG_DIR          0b10100000
#define ST7567_LCD_CMD_COM_DIR          0b11000000
#define ST7567_LCD_CMD_COM_DIR_NOR      0b00000000
#define ST7567_LCD_CMD_COM_DIR_REV      0b00001000
#define ST7567_LCD_CMD_REG_RATIO        0b00100000 
#define ST7567_LCD_CMD_EV1              0b10000001
#define ST7567_LCD_CMD_EV2              0b00000000
#define ST7567_LCD_CMD_PWR_CTRL         0b00101000
#define ST7567_LCD_CMD_PWR_CTRL_VB      0b00000100
#define ST7567_LCD_CMD_PWR_CTRL_VR      0b00000010
#define ST7567_LCD_CMD_PWR_CTRL_VF      0b00000001
#define ST7567_LCD_CMD_LCD_ENABLE       0b10101110
#define ST7567_LCD_CMD_LCD_ENABLE_ON    0b00000001
#define ST7567_LCD_CMD_LCD_ENABLE_OFF   0b00000000
#define ST7567_LCD_CMD_START_LINE       0b01000000
#define ST7567_LCD_CMD_START_PAGE_ADDR  0b10110000
#define ST7567_LCD_CMD_SET_COL_ADDR_1   0b00010000
#define ST7567_LCD_CMD_SET_COL_ADDR_2   0b00000000

// "Stolen" from Adafruit GFX.
#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

// LCD Driver Class.
class ST7567_LCD : public Adafruit_GFX
{
  // Public methods and varibles.
  public:
    // Class constructor.
    ST7567_LCD(SPIClass *_lcdSPI = &SPI);
    void begin(int _csPin, int _dcPin, int _rstPin);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void display();
    void clearDisplay();
    void setContrast(uint8_t _contrast);
    void setRotation(int _rotation);

  // Private methods and variables.
  private:
    void initLCD();
    void lcdReset();
    void setCommand();
    void setData();
    void spiSend(uint8_t *_data, uint16_t _n);

    // SPI class pinter for display communication.
    SPIClass *_spiPtr = NULL;
    SPISettings _lcdSettings;

    // LCD Framebuffer.
    uint8_t _lcdFrameBuffer[ST7567_LCD_WIDTH * ST7567_LCD_HEIGHT / 8];

    // LCD SPI Chip Select pin.
    int _cs = -1;

    // LCD Reset pin.
    int _rst = -1;

    // LCD Data/Command pin.
    int _dc = -1;

    // Contrast value, by default set it to 52.
    uint8_t _contrastValue = 52;

    // Adafruit GFX rotation variable.
    uint8_t rotation;
};

#endif