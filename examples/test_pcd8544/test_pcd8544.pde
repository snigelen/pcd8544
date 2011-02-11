// Example of how to use the pcd8544 library, the lcd-driver used in
// Nokia 3310 and others.
//
// Tested on Sparkfun's "Graphic LCD 84x48 - Nokia 5110".

#include "pcd8544.h"


// Some Aruino/Maple specific stuff
#ifdef BOARD_maple
#define ADMAX 4095
#define ADPIN 15
#define Serial SerialUSB
#else // Arduino
#define ADMAX 1023
#define ADPIN 0
#endif


// Create a pcd8544 object.
// Parameters are: dc_pin, reset_pin, cs_pin
// Hardware SPI will be used (SPI1 on the Maple).
// sdin (MISO) is on pin 13 and sclk on pin 11.
pcd8544 lcd(5, 6, 7);


void setup(void)
{
#ifdef BOARD_maple
	pinMode(ADPIN, INPUT_ANALOG);
#else
	Serial.begin(115200);
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
	for (uint8_t i = 0; i < LCD_WIDTH/2-2; i++)
		lcd.data(0b00010100);
	lcd.data(0); // A blink pixel column

	// Some small numbers
	for (uint8_t i = 0; i < 11; i++)
		lcd.smallNum(i);

	lcd.setCursor(0, 3);
	lcd.println("   PCD8544");
	lcd.println("      on  ");
#ifdef BOARD_maple
	lcd.print("  the Maple");
#else
	lcd.print("   Arduino");
#endif
}



void loop(void)
{
	uint8_t i, val;

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

