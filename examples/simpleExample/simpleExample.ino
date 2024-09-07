// Include ST7567 LCD libary.
#include <ST7567.h>

// Use defalt SPI on ESP32 (VSPI) - (MOSI = 23, MISO = 19, SCK = 18).
ST7567_LCD lcd;

// LCD pins (i this case for ESP32).
#define LCD_CS  2
#define LCD_RST 4
#define LCD_DC  5

void setup()
{
    // Initialize LCD.
    lcd.begin(LCD_CS, LCD_DC, LCD_RST);

    // Set text to black and text background to white.
    lcd.setTextColor(1, 0);

    // Set text scale to 1.
    lcd.setTextSize(1);

    // Set text cursor to X = 0, Y = 0.
    lcd.setCursor(0, 0);

    // Print some string.
    lcd.print("This is some sample\ntext! :)");
    lcd.display();
    
    // Wait a little bit and clear display.
    delay(2000);
    lcd.clearDisplay();
}

void loop()
{
    // Display millis() value as fast as possible.
    lcd.setCursor(0, 0);
    lcd.print(millis(), DEC);
    lcd.display();
}
