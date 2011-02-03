#include "WProgram.h"

#ifdef BOARD_maple
#include <spi.h>
#define PROGMEM __attribute__ ((section (".USER_FLASH")))
#define pgm_read_byte(adr) *(adr)

#else  // Arduino stuff
#include <avr/pgmspace.h>
#endif

#include <stdint.h>
#include "pcd8544.h"


// LCD commands, Table 1, page 14
#define LCD_FUNCTION_SET (1<<5)
#define LCD_FUNCTION_PD  (1<<2)
#define LCD_FUNCTION_V   (1<<1)
#define LCD_FUNCTION_H   (1<<0)

// Normal instructions, H = 0
#define LCD_DISPLAY_CONTROL (1<<3)
#define LCD_DISPLAY_CONTROL_D (1<<2)
#define LCD_DISPLAY_CONTROL_E (1<<0)
#define LCD_DISPLAY_CONTROL_BLANK 0
#define LCD_DISPLAY_CONTROL_NORMAL_MODE  LCD_DISPLAY_CONTROL_D
#define LCD_DISPLAY_CONTROL_ALL_ON       LCD_DISPLAY_CONTROL_E
#define LCD_DISPLAY_CONTROL_INVERSE      (LCD_DISPLAY_CONTROL_D|LCD_DISPLAY_CONTROL_E)

#define LCD_SET_Y_ADDRESS (1<<6)
#define LCD_Y_ADRESS_MASK 0b111
#define LCD_SET_X_ADDRESS (1<<7)
#define LCD_X_ADRESS_MASK 0b01111111

// Extended instructions. H = 1
#define LCD_TEMP_CONTROL (1<<2)
#define LCD_TEMP_TC1     (1<<1)
#define LCD_TEMP_TC0     (1<<0)

#define LCD_BIAS     (1<<4)
#define LCD_BIAS_BS2 (1<<2)
#define LCD_BIAS_BS1 (1<<1)
#define LCD_BIAS_BS0 (1<<0)

#define LCD_VOP (1<<7)


#if USE_FONT_8x14
#include "Fixed_8x13.h"
#endif


const unsigned char PROGMEM small_num[][4] = {
        {0x0e,0x15,0x0e,0x00}, // 48, zero
        {0x12,0x1f,0x10,0x00}, // 49, one
        {0x12,0x19,0x16,0x00}, // 50, two
        {0x11,0x15,0x0b,0x00}, // 51, three
        {0x07,0x04,0x1f,0x00}, // 52, four
        {0x17,0x15,0x09,0x00}, // 53, five
        {0x0e,0x15,0x09,0x00}, // 54, six
        {0x19,0x05,0x03,0x00}, // 55, seven
        {0x1a,0x15,0x0b,0x00}, // 56, eight
        {0x12,0x15,0x0e,0x00}, // 57, nine
        {0x00,0x10,0x00,0x00}, // 46, period
};


const unsigned char PROGMEM font5x7 [][5]  =  {
        { 0x00, 0x00, 0x00, 0x00, 0x00 },   // space
	{ 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
	{ 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
	{ 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
	{ 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
        { 0x32, 0x34, 0x08, 0x16, 0x26 },   // %
	{ 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
	{ 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
	{ 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
	{ 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
	{ 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
	{ 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
	{ 0x00, 0x00, 0x50, 0x30, 0x00 },   // ,
	{ 0x10, 0x10, 0x10, 0x10, 0x10 },   // -
	{ 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
	{ 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
	{ 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
	{ 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
	{ 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
	{ 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
	{ 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
	{ 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
	{ 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
	{ 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
	{ 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
	{ 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
	{ 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
	{ 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
	{ 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
	{ 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
	{ 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
	{ 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
	{ 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
	{ 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A
	{ 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
	{ 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
	{ 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
	{ 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
	{ 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
	{ 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
	{ 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
	{ 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
	{ 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
	{ 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
	{ 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
	{ 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
	{ 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
	{ 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
	{ 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
	{ 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
	{ 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
	{ 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
	{ 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
	{ 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
	{ 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
	{ 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
	{ 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
	{ 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
	{ 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
	{ 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
	{ 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55
	{ 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
	{ 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
	{ 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
	{ 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
	{ 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
	{ 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
	{ 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
	{ 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
	{ 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
	{ 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
	{ 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g
	{ 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
	{ 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
	{ 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j
	{ 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
	{ 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
	{ 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
	{ 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
	{ 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
	{ 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p
	{ 0x08, 0x14, 0x14, 0x18, 0x7C },   // q
	{ 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
	{ 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
	{ 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
	{ 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
	{ 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
	{ 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
	{ 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
	{ 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y
	{ 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
	{ 0x00, 0x7F, 0x3E, 0x1C, 0x08 },   // > Filled
        { 0x08, 0x1C, 0x3E, 0x7F, 0x00 },   // < Filled
        { 0x08, 0x7C, 0x7E, 0x7C, 0x08 },   // Arrow up
        { 0x10, 0x3E, 0x7E, 0x3E, 0x10 },   // Arrow down       
        { 0x3E, 0x3E, 0x3E, 0x3E, 0x3E },   // Stop
        { 0x00, 0x7F, 0x3E, 0x1C, 0x08 }    // Play
};


uint8_t lcd_current_row, lcd_current_column;

void lcd_inc_rc(void)
{
	if (++lcd_current_column >= LCD_WIDTH) {
		lcd_current_column = 0;
		if (++lcd_current_row >= LCD_LINES)
			    lcd_current_row = 0;
	}
}

void lcd_write(uint8_t dc, uint8_t data)
{
	digitalWrite(PIN_DC, dc);
	digitalWrite(PIN_SCE, LOW);
#if SOFT_SPI
	shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
#else
#ifdef BOARD_maple
	spi_tx_byte(1, data);
#else
	SPDR = data;
	while(!(SPSR & (1<<SPIF))) ;
#endif
#endif
	digitalWrite(PIN_SCE, HIGH);
	if(dc)
		lcd_inc_rc();
}


void lcd_init(void)
{
	pinMode(PIN_SCE,   OUTPUT);
	pinMode(PIN_RESET, OUTPUT);
	pinMode(PIN_DC,    OUTPUT);
	pinMode(PIN_SDIN,  OUTPUT);
	pinMode(PIN_SCLK,  OUTPUT);

#ifdef BOARD_maple
	timer_set_mode(TIMER3, 2, TIMER_DISABLED);
        timer_set_mode(TIMER3, 1, TIMER_DISABLED);
#endif

	
#if ! SOFT_SPI
#ifdef BOARD_maple
	spi_init(1, SPI_PRESCALE_16, SPI_MSBFIRST, 0);
#else
	pinMode(10, OUTPUT); // To ensure master mode
	SPCR |= (1<<SPE) | (1<<MSTR);
#endif
#endif
	digitalWrite(PIN_RESET, LOW);
	delay(1);
	digitalWrite(PIN_RESET, HIGH);
  
	// Extenden instructions and !powerdown
	// and horizontal adressing (autoincrement of x-adress)
	lcd_cmd(LCD_FUNCTION_SET | LCD_FUNCTION_H);
	// Set Vop to 0x3F
	lcd_cmd(LCD_VOP | 0x3F);
	// Vlcd temp. coeff. 0
	lcd_cmd(LCD_TEMP_CONTROL);
	// Bias system 4, 1:48
	lcd_cmd(LCD_BIAS | LCD_BIAS_BS1 | LCD_BIAS_BS0);
	// Set H = 0 for normal instructions
	lcd_cmd(LCD_FUNCTION_SET);  
	// Normal mode
	lcd_cmd(LCD_DISPLAY_CONTROL | LCD_DISPLAY_CONTROL_NORMAL_MODE);
}


void lcd_clear(void)
{
	int i;
	for (i = 0; i < LCD_WIDTH*LCD_LINES; i++)
		lcd_data(0);
}


void lcd_goto(uint8_t row, uint8_t column)
{
	if (row >= LCD_LINES)
		row %= LCD_LINES;
	lcd_cmd(LCD_SET_X_ADDRESS | column);
	lcd_cmd(LCD_SET_Y_ADDRESS | row);
	lcd_current_row = row;
	lcd_current_column = column;
}


void lcd_char(char ch)
{
	uint8_t i;
	for (i = 0; i < 5; i++)
		lcd_data(pgm_read_byte(&font5x7[ch-' '][i]) <<1);
	lcd_data(0);
}


void lcd_smallnum(uint8_t num, uint8_t shift)
{
	uint8_t i;
	for (i = 0; i < 4; i++)
		lcd_data(pgm_read_byte(&small_num[num][i])<<shift);
}

void lcd_string(char *s)
{
	while (*s)
		lcd_char(*s++);
}

#if USE_FONT_8x14
// Draw a char higher than 8 pixels. Uses data in row order.
// Upper left corner of a char is on current row.
void lcd_big_char(char ch)
{
	uint8_t cp, rp, r, c, i
		startrow = lcd_current_row,
		column   = lcd_current_column;
	for (r = 0; r < 2; r++) {
		lcd_goto(startrow + r, column);
		for (c = 0; c < 8; c++) {
			cp = 0;
			for (i = 0; i < (r<1?8:5); i++) {
				rp = pgm_read_byte(&Fixed_8x13[ch-' '][i+8*r]);
				if (rp & (0x80>>c))
					cp |= 1<<i;
			}
			lcd_data(cp);
		}
	}
	lcd_goto(startrow, 8+column);
}

void lcd_big_string(char *s)
{
	while (*s)
		lcd_big_char(*s++);
}
#endif


void lcd_clear_rest_of_line(void)
{
	while (lcd_current_column != 0)
		lcd_data(0);
}
