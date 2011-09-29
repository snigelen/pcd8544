// Lets throw some dice.
//
// Look at test_pcd8544 first.

#include "pcd8544.h"

#ifndef __AVR-GCC__
#define PROGMEM __attribute__ ((section (".USER_FLASH")))
#define ADMAX 4095
#define ADPIN 15
#else // Arduino
#include <avr/pgmspace.h>
#define ADMAX 1023
#define ADPIN 0
#endif


pcd8544 lcd(5, 6, 7);


// 15 pixel wide 2 rows high die.
uint8_t die1[2*15] PROGMEM = {
	0xFF,0x01,0x01,0x01,0x01,0x01,0xC1,0xC1,0xC1,0x01,0x01,0x01,0x01,
	0x01,0xFF,0x7F,0x40,0x40,0x40,0x40,0x40,0x41,0x41,0x41,0x40,0x40,
	0x40,0x40,0x40,0x7F};

uint8_t die2[] PROGMEM = {
	0xFF,0x01,0x1D,0x1D,0x1D,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0xFF,0x7F,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x5C,
	0x5C,0x5C,0x40,0x7F};

uint8_t die3[] PROGMEM = {
	0xFF,0x01,0x1D,0x1D,0x1D,0x01,0xC1,0xC1,0xC1,0x01,0x01,0x01,0x01,
	0x01,0xFF,0x7F,0x40,0x40,0x40,0x40,0x40,0x41,0x41,0x41,0x40,0x5C,
	0x5C,0x5C,0x40,0x7F};

uint8_t die4[] PROGMEM = {
	0xFF,0x01,0x1D,0x1D,0x1D,0x01,0x01,0x01,0x01,0x01,0x1D,0x1D,0x1D,
	0x01,0xFF,0x7F,0x40,0x5c,0x5c,0x5c,0x40,0x40,0x40,0x40,0x40,0x5C,
	0x5C,0x5C,0x40,0x7F};

uint8_t die5[] PROGMEM = {
	0xFF,0x01,0x1D,0x1D,0x1D,0x01,0xC1,0xC1,0xC1,0x01,0x1D,0x1D,0x1D,
	0x01,0xFF,0x7F,0x40,0x5c,0x5c,0x5c,0x40,0x41,0x41,0x41,0x40,0x5C,
	0x5C,0x5C,0x40,0x7F};

uint8_t die6[] PROGMEM = {
	0xFF,0x01,0xDD,0xDD,0xDD,0x01,0x01,0x01,0x01,0x01,0xDD,0xDD,0xDD,
	0x01,0xFF,0x7F,0x40,0x5D,0x5D,0x5D,0x40,0x40,0x40,0x40,0x40,0x5D,
	0x5D,0x5D,0x40,0x7F};


// An array with all 6 bitmaps
uint8_t *die[6] = {die1, die2, die3, die4, die5, die6};
	

void draw_die(uint8_t value)
{
	if (value > 6 || value < 1)
		return;

	lcd.bitmap(die[value-1], 2, 15); 
}


void setup(void)
{
	uint8_t i;

	// We don't need to do this on Arduino
#ifdef BOARD_maple
	pinMode(ADPIN, INPUT_ANALOG);
#endif

	lcd.begin();
	lcd.clear();
	lcd.println("  Lets play!");
	lcd.print(" Speed=ADPIN");

	// Draw 1 to 3
	for (i = 1; i <= 3; i++) {
		lcd.gotoRc(2, 16*i+1);
		draw_die(i);
	}
	// 4 to 6
	for (i = 1; i <= 3; i++) {
		lcd.gotoRc(4, 16*i+1);
		draw_die(i+3);
	}
	delay(3000);

	lcd.clear();

	// Small numbers for the histogram
	lcd.gotoRc(5, 45);
	for (i = 1; i <= 6; i++)
		lcd.smallNum(i);

	// Make sure we don't get exactly the same sequence every time
	randomSeed(analogRead(ADPIN));
}


void loop(void)
{
	uint8_t value, i;

	// Make theese static so the values are remembered.
	// They are all initialized to 0.
	static unsigned int n;
	static int outcomes[6], max;

	// Draw a random integer between 0 and 5
	value = random(0,6);

	// Store the number of different outcomes
	outcomes[value]++;
	if (outcomes[value] > max)
		max = outcomes[value];
	
	// Show the number at character position 0 to 13
	lcd.setCursor(n%14, 0);
	n = n+1;
	lcd.print(value+1, DEC);
	lcd.clearRestOfLine();

	// Total number
	lcd.setCursor(0, 1);
	lcd.print("n=");
	lcd.print(n, DEC);

	// Draw a die
	lcd.gotoRc(3,16);
	draw_die(value+1);

	// Make a histogram
	for (i = 0; i < 6; i++) {
		lcd_bar(4, 45 + 4*i,
			map(outcomes[i], 0, max, 0, 22),
			3, 22);
	}

	// Delay proportional to analog pin.
	delay(analogRead(ADPIN) * 1000L/ADMAX);
}


// Draw a vertical bar. Maybe make this part of the library.
void lcd_bar(uint8_t bottom_line, uint8_t start_col, uint8_t height,
	     uint8_t width, uint8_t maxheight)
{
	uint8_t i, h, rest, w;
	
	h = height/8;
	rest = height-8*h;

	i = 0;
	// Draw completely filled
	while(i < h) {
		lcd.gotoRc(bottom_line-i, start_col);
		for (w = 0; w < width; w++)
			lcd.data(0xFF);
		i++;
	}
	// Draw parly filled
	if (rest) {
		lcd.gotoRc(bottom_line-i, start_col);
		for (w = 0; w < width; w++)
			lcd.data(0xFF<<(8-rest));
		i++;
	}
	// Clear rest
	while(i < maxheight/8 + 1) {
		lcd.gotoRc(bottom_line-i, start_col);
		for (w = 0; w < width; w++)
			lcd.data(0);
		i++;
	}
}
