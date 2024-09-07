// Include main header file.
#include "ST7567.h"

// Include Adafruit GFX here as well
#include "Adafruit_GFX/Adafruit_GFX.h"

// LCD Library constructor.
ST7567_LCD::ST7567_LCD(SPIClass *_lcdSPI) : Adafruit_GFX(ST7567_LCD_WIDTH, ST7567_LCD_HEIGHT)
{
    _spiPtr = _lcdSPI;
}

void ST7567_LCD::begin(int _csPin, int _dcPin, int _rstPin)
{
    // Save pin numbers locally.
    _cs = _csPin;
    _rst = _rstPin;
    _dc = _dcPin;

    // Create object for SPI settings to this specific LCD.
    _lcdSettings = SPISettings(8000000ULL, MSBFIRST, SPI_MODE3);

    // Set them as outputs.
    pinMode(_cs, OUTPUT);
    pinMode(_rst, OUTPUT);
    pinMode(_dc, OUTPUT);

    // Disable the SPI on LCD.
    digitalWrite(_cs, HIGH);

    // Initialize SPI library.
    _spiPtr->begin();
    
    // Reset the LCD.
    lcdReset();

    // Initalize LCD.
    initLCD();
}

void ST7567_LCD::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    // Check for display area.
    if ((x < 0) || (x > (width() - 1)) || (y < 0) || (y > (height()))) return;

    // Limit color only to B/W.
    color &= 1;

    // Rotate if needed.
    switch (rotation)
    {
        case 0:
          // Do nothing, default rotation.
          break;
        case 1:
          _swap_int16_t(x, y);
          x = height() - x - 1;
          break;
        case 2:
          x = width() - x - 1;
          y = height() - y - 1;
          break;
        case 3:
          _swap_int16_t(x, y);
          y = width() - y - 1;
          break;
    }

    _lcdFrameBuffer[x + ((y / 8) * 128)] &= ~(1 << (y % 8));
    _lcdFrameBuffer[x + ((y / 8) * 128)] |= (color << (y % 8));
}

void ST7567_LCD::display()
{
    for (int i = 0; i < 8; i++)
    {
        // Set to command mode.
        setCommand();

        // Set current page.
        uint8_t _lcdStartPageAddr = ST7567_LCD_CMD_START_PAGE_ADDR | i;
        spiSend(&_lcdStartPageAddr, 1);

        // Set everything to the start of the screen.
        uint8_t _lcdStartLine = ST7567_LCD_CMD_START_LINE;
        spiSend(&_lcdStartLine, 1);

        uint8_t _lcdSetCol[2] = {ST7567_LCD_CMD_SET_COL_ADDR_1, ST7567_LCD_CMD_SET_COL_ADDR_2};
        spiSend(_lcdSetCol, 2);

        // Send data for this page.
        // Send data to the LCD.
        setData();
        spiSend(_lcdFrameBuffer + (i * 128), 128);

        // Set command to end data send.
        setCommand();
    }
}

void ST7567_LCD::clearDisplay()
{
    memset(_lcdFrameBuffer, 0, sizeof(_lcdFrameBuffer));
}

void ST7567_LCD::setContrast(uint8_t _contrast)
{
    // Limit value to max 63;
    _contrastValue &= 63;

    // Save it locally.
    _contrastValue = _contrast;

    uint8_t _lcdEv[2] = {ST7567_LCD_CMD_EV1, ST7567_LCD_CMD_EV2 | _contrastValue};
    spiSend(_lcdEv, 2);
}

void ST7567_LCD::setRotation(int _rotation)
{
    rotation = _rotation % 4;

    switch (rotation)
    {
    case 0:
    case 2:
        _width = WIDTH;
        _height = HEIGHT;
        break;
    case 1:
    case 3:
        _width = HEIGHT;
        _height = WIDTH;
        break;
    }
}


void ST7567_LCD::initLCD()
{
    // Select command mode.
    setCommand();

    // Set LCD bias setting to 1/7.
    uint8_t _lcdBiasSet = ST7567_LCD_CMD_BIAS_SELECT | 1;
    spiSend(&_lcdBiasSet, 1);

    // Set Segment direction to normal.
    uint8_t _lcdSegDir = ST7567_LCD_CMD_SEG_DIR;
    spiSend(&_lcdSegDir, 1);

    // Set COM direction to normal.
    uint8_t _lcdComDir = ST7567_LCD_CMD_COM_DIR | ST7567_LCD_CMD_COM_DIR_REV;
    spiSend(&_lcdComDir, 1);

    // Set regulation ratio.
    uint8_t _lcdRegRatio = ST7567_LCD_CMD_REG_RATIO | 4;
    spiSend(&_lcdRegRatio, 1);

    // Set EV Value - Contrast, I guess? This is a two byte command!
    uint8_t _lcdEv[2] = {ST7567_LCD_CMD_EV1, ST7567_LCD_CMD_EV2 | _contrastValue};
    spiSend(_lcdEv, 2);

    // Set Power control - enable booster, regulator, follower.
    uint8_t _lcdPwrCtrl = ST7567_LCD_CMD_PWR_CTRL | ST7567_LCD_CMD_PWR_CTRL_VB | ST7567_LCD_CMD_PWR_CTRL_VR | ST7567_LCD_CMD_PWR_CTRL_VF;
    spiSend(&_lcdPwrCtrl, 2);

    // Initalize DRAM.
    clearDisplay();
    display();

    // Power up the display.
    uint8_t _lcdPowerOn = ST7567_LCD_CMD_LCD_ENABLE | ST7567_LCD_CMD_LCD_ENABLE_ON;
    spiSend(&_lcdPowerOn, 1);
}

void ST7567_LCD::lcdReset(void)
{
    // Set reset pin to low to initiate reset.
    digitalWrite(_rst, LOW);

    // Datasheet states >5uS kepp the reset line low.
    // Keep it at least 1 milliseconds, to be sure.
    delay(1);

    // Release the Reset line.
    digitalWrite(_rst, HIGH);

    // Datasheet states >5uS kepp the reset line low.
    // Keep it at least 1 milliseconds, to be sure.
    delay(1);
}

void ST7567_LCD::setCommand(void)
{
    digitalWrite(_dc, LOW);
}

void ST7567_LCD::setData(void)
{
    digitalWrite(_dc, HIGH);
}

void ST7567_LCD::spiSend(uint8_t *_data, uint16_t _n)
{
    // Enable CS.
    digitalWrite(_cs, LOW);

    _spiPtr->beginTransaction(_lcdSettings);
#ifdef ARDUINO_ESP32_DEV
    _spiPtr->writeBytes(_data, _n);
#else
    for (int i = 0; i < _n; i++)
    {
        _spiPtr->transfer(_data[i]);
    }
#endif
    _spiPtr->endTransaction();

    // Disable CS line.
    digitalWrite(_cs, HIGH);
}