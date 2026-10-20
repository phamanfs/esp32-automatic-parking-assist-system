#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

void lcdDisplayInit();
void lcdDisplayShow(const char* line1, const char* line2);
void lcdDisplayShowParkingStatus(const char* mode, const char* step);
void lcdDisplayUpdateDistances();
void lcdDisplayShowManual();

#endif
