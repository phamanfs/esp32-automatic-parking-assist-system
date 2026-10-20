#include "LcdDisplay.h"

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "Config.h"
#include "Pins.h"
#include "Ultrasonic.h"

static LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);
static unsigned long lastDistanceUpdateMs = 0;
static const unsigned long DISTANCE_UPDATE_INTERVAL_MS = 250;

static void writeLine(uint8_t row, const char* text) {
  lcd.setCursor(0, row);
  bool endOfText = text == nullptr;
  for (uint8_t column = 0; column < LCD_COLUMNS; ++column) {
    char character = ' ';
    if (!endOfText) {
      character = text[column];
      if (character == '\0') {
        endOfText = true;
        character = ' ';
      }
    }
    lcd.print(character);
  }
}

void lcdDisplayInit() {
  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void lcdDisplayShow(const char* line1, const char* line2) {
  writeLine(0, line1);
  if (LCD_ROWS > 1) {
    writeLine(1, line2);
  }
}

void lcdDisplayShowParkingStatus(const char* mode, const char* step) {
  char line1[LCD_COLUMNS + 1];
  char line2[LCD_COLUMNS + 1];
  snprintf(line1, sizeof(line1), "Mode: %s", mode);
  snprintf(line2, sizeof(line2), "Step: %s", step);
  lcdDisplayShow(line1, line2);
}

static void formatDistance(char* buffer, size_t size, float distance) {
  if (distance > 0.0f) {
    snprintf(buffer, size, "%.1f", distance);
  } else {
    snprintf(buffer, size, "--.-");
  }
}

void lcdDisplayUpdateDistances() {
  if (LCD_ROWS < 4 || millis() - lastDistanceUpdateMs < DISTANCE_UPDATE_INTERVAL_MS) {
    return;
  }

  lastDistanceUpdateMs = millis();

  char front[8];
  char rear[8];
  char right[8];
  char line3[LCD_COLUMNS + 1];
  char line4[LCD_COLUMNS + 1];
  formatDistance(front, sizeof(front), getFrontDistance());
  formatDistance(rear, sizeof(rear), getRearDistance());
  formatDistance(right, sizeof(right), getRightDistance());

  snprintf(line3, sizeof(line3), "F:%s R:%s", front, rear);
  snprintf(line4, sizeof(line4), "Right: %s", right);
  writeLine(2, line3);
  writeLine(3, line4);
}

void lcdDisplayShowManual() {
  lcdDisplayShow("Mode: Bluetooth", "Ready to find slot");
}
