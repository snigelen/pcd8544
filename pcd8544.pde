#include "pcd8544.h"

#ifdef BOARD_maple
#define ADMAX 4095
#define ADPIN 15
#else  // Arduino
#define ADMAX 1023
#define ADPIN 0
#endif


void setup(void)
{
	uint8_t i;
	lcd_init();
	lcd_clear();

	// Fill the screen with characters
	for (char ch = '!'; ch <= 't'; ch++)
		lcd_char(ch);
	delay(2000);

	lcd_clear();

	lcd_goto(0,0);
	lcd_string("Level: ");
	lcd_goto(1,0);
	lcd_string("Temp: ");

	lcd_goto(2,0);
	// Draw a double line
	for (i = 0; i < 22; i++)
		lcd_data(0b00010100);
	lcd_data(0); // A blank column
	// Small numbers
	for (i = 0; i < 10; i++)
		lcd_smallnum(i, 1);

	// Fill reest of row with a double line
	i = lcd_current_row;
	while (i == lcd_current_row)
		lcd_data(0b00010100);


	lcd_goto(3, 4); lcd_string("   PCD8522");
	lcd_goto(4, 0); lcd_string("      on  ");
#ifdef BOARD_maple
	lcd_goto(5, 4); lcd_string("  the Maple");
#else
	lcd_goto(5, 4); lcd_string("   Arduino");
#endif
}



void loop(void)
{
	uint8_t i, val;

	// Draw bar on line 0
	lcd_goto(0, 8*5);
	val = map(analogRead(ADPIN), 0, ADMAX, 8*5, 84);

	for (i = 5*8; i < val; i++)
		lcd_data(0b01111100); // Bar
	lcd_clear_rest_of_line();

	// Write fake temperature
	lcd_goto(1, 8*5);
	lcd_string("22.6");
	lcd_data(0);
	lcd_data(0b00000100); // Degree sign
	lcd_data(0b00001010);
	lcd_data(0b00000100);
	lcd_data(0);
	lcd_char('C');
	lcd_clear_rest_of_line();

	// The bar looks a litle strange if updated to often
	delay(100);  
}
