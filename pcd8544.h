#ifndef PCD8544_H
#define PCD8544_H

#include <stdint.h>

// Set SOFT_SPI to 1 for software SPI using shiftOut().
// -> SCLK 534 kHz on Maple and 66 kHz on Arduino(@16MHz).
// Set to 0 for hardware SPI.
// -> SCLK 4.5 MHz on Maple and 4 MHz on Arduino(@16MHz).
// The datasheet says max 4 MHz, but it seems to work well even at 9 MHz.
// (but not at 18 MHz).

#define SOFT_SPI 0

#define USE_FONT_8x14 0

#define PIN_SCE    7
#define PIN_RESET  6
#define PIN_DC     5
#define PIN_SDIN  11  // MOSI
#define PIN_SCLK  13

#define LCD_LINES 6
#define LCD_COLS  14
#define LCD_WIDTH  84
#define LCD_HEIGHT 48


#define lcd_cmd(cmd)   lcd_write(0, cmd)
#define lcd_data(data) lcd_write(1, data)


extern uint8_t lcd_current_row, lcd_current_column;


#ifdef __cplusplus
extern "C" {
#endif

// Call me first
void lcd_init(void);

// Write command (dc = 0) or data (dc = 1) to display
void lcd_write(uint8_t dc, uint8_t data);

// Guess what! Clear the display. Doesn't change current location.
void lcd_clear(void);


// Put a 5x7 character at current location. (and a one pixel blank).
void lcd_char(char ch);
void lcd_string(char *s);

void lcd_smallnum(uint8_t num, uint8_t shift);

#if USE_FONT_8x14
void lcd_big_char(char ch);
void lcd_big_string(char *s);
#endif

void lcd_clear_rest_of_line(void);

// Set location to row, column.
// 0 <= row <= 5 and 0 <= column <= 83
void lcd_goto(uint8_t row, uint8_t column);

#ifdef __cplusplus
}
#endif

#endif
