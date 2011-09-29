// Example of how to use the pcd8544 library, the lcd-driver used in
// Nokia 3310 and others.
//
// Tested on Sparkfun's "Graphic LCD 84x48 - Nokia 5110".

#include "pcd8544.h"

/*
  Display, top row left to right - Maple/Arduino  Mega

  Vcc                            - 3.3V
  GND                            - GND
  SCE - chip enable/select       - 7
  RST                            - 6
  D/C                            - 5
  DN - MOSI                      - 11             51
  SCLK                           - 13             52
  LED                            - resistor - Vcc or unconnected
 */

// Some Aruino/Maple specific stuff
#ifndef __AVR-GCC__
#define ADMAX 4095
#define ADPIN 15
#define PROGMEM
#else // Arduino
#define ADMAX 1023
#define ADPIN 0
#include <avr/pgmspace.h>
#endif

byte dc_pin = 5;    // Data/Command
byte reset_pin = 6;
byte cs_pin = 7;    // Chip select (SCE)

// Create a pcd8544 object.
// Hardware SPI will be used.
// (SPI1 on the Maple, add 2 as last argument for SPI2 (untested)).
// sdin (MOSI) is on pin 11 and sclk on pin 13.
pcd8544 lcd(dc_pin, reset_pin, cs_pin);

// Use 5 arguments for soft SPI. Any pins can be choosen.
// byte sdin_pin = 11, sclk_pin = 13;
// pcd8544 lcd(dc_pin, reset_pin, cs_pin, sdin_pin, sclk_pin);


// 2 rows by 16 cols smiley. First 16 bytes is first row,
// next 16 is second.
byte smile[] PROGMEM = {
	0xE0,0x18,0x04,0x04,0x02,0x32,0x32,0x02,0x02,0x22,0x22,
	0x02,0x04,0x04,0x18,0xE0,0x07,0x18,0x20,0x22,0x44,0x48,
	0x48,0x48,0x48,0x48,0x48,0x44,0x22,0x20,0x18,0x07};


void setup(void)
{
#ifdef BOARD_maple
	pinMode(ADPIN, INPUT_ANALOG);
#endif
	lcd.begin();  // Always call lcd.begin() first.
	lcd.clear();

	// Fill the screen with characters
	for (char ch = '!'; ch <= 't'; ch++)
		lcd.print(ch);
	delay(2000);

	lcd.clear();
	// Set cursor to (character) column zero and row 0
	lcd.setCursor(0,0);
	lcd.println("Level: ");  // First line
	lcd.println("Temp: ");   // println skips to second line

	// Draw a double line
	for (byte i = 0; i < PCD8544_WIDTH/2-2; i++)
		lcd.data(0b00010100);
	lcd.data(0); // A blink pixel column

	// Some small numbers
	for (byte i = 0; i < 11; i++)
		lcd.smallNum(i);

	lcd.setCursor(0, 3);
	lcd.println(" PCD8544");
	lcd.println("    on");
#ifdef BOARD_maple
	lcd.print("the Maple");
#else
	lcd.print(" Arduino");
#endif
	// Draw an image.
	lcd.gotoRc(4, 57);
	lcd.bitmap(smile, 2, 16);
}



void loop(void)
{
	byte i, val;

	// Draw bar on row 0, pixel column 8*5
	lcd.gotoRc(0, 8*5);
	val = map(analogRead(ADPIN), 0, ADMAX, 8*5, 84);

        // the bar
	for (i = 5*8; i < val; i++)
		lcd.data(0b01111100); // Bar
	lcd.clearRestOfLine();

	// Write a fake temperature
	lcd.gotoRc(1, 8*5);
	lcd.print("22.8");
	lcd.data(0);

	// A degree sign
	lcd.data(0b00000100); // Degree
	lcd.data(0b00001010);
	lcd.data(0b00000100);
	lcd.data(0);
	lcd.print('C');
	lcd.clearRestOfLine();
	delay(100);
}

