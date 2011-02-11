#include "pcd8544.h"

#ifdef BOARD_maple
#define ADMAX 4095
#define ADPIN 15
#define Serial SerialUSB
#else // Arduino
#define ADMAX 1023
#define ADPIN 0
#endif

pcd8544 lcd(5, 6, 7);

void setup(void)
{
#ifdef BOARD_maple
	pinMode(ADPIN, ANALOG_INPUT);
#else
	Serial.begin(115200);
#endif
	lcd.begin();
	lcd.clear();

	// Fill the screen with characters
	for (char ch = '!'; ch <= 't'; ch++)
		lcd.print(ch);
	delay(1000);

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.println("Level: ");
	lcd.setCursor(0,1);
	lcd.println("Temp: ");
	lcd.setCursor(0,2);
	for (uint8_t i = 0; i < LCD_WIDTH/2-2; i++)
		lcd.data(0b00010100);
	lcd.data(0);
	for (uint8_t i = 0; i < 11; i++)
		lcd.smallNum(i);

	lcd.gotoRc(3, 4); lcd.print("   PCD8522");
	lcd.gotoRc(4, 0); lcd.print("      on  ");
#ifdef BOARD_maple
	lcd.gotoRc(5, 4); lcd.print("  the Maple");
#else
	lcd.gotoRc(5, 4); lcd.print("   Arduino");
#endif
}



void loop(void)
{
	uint8_t i, val;

	// Draw bar on line 0
	lcd.gotoRc(0, 8*5);
	val = map(analogRead(ADPIN), 0, ADMAX, 8*5, 84);
//	Serial.println(val, DEC); delay(200);
	for (i = 5*8; i < val; i++)
		lcd.data(0b01111100); // Bar
	//lcd_clear_rest_of_line();

	// Write temperature
	lcd.gotoRc(1, 8*5);
	lcd.print("22.8");
	lcd.data(0);
	lcd.data(0b00000100); // Degree
	lcd.data(0b00001010);
	lcd.data(0b00000100);
	lcd.data(0);
	lcd.print('C');
	delay(100);
}

