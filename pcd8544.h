#ifndef PCD8544_H
#define PCD8544_H

#include <stdint.h>
#include <Print.h>

class pcd8544 : public Print {
public:
	pcd8544(uint8_t dc_pin, uint8_t reset_pin, uint8_t cs_pin, uint8_t hardware_spi = 1);
	void begin(void);
	void clear(void);
	void setCursor(uint8_t column, uint8_t row);
	void gotoRc(uint8_t row, uint8_t pixel_column);
	void data(uint8_t data);
	void smallNum(uint8_t num, uint8_t shift = 1);
	void clearRestOfLine(void);

private:
	void send(uint8_t dc, uint8_t data);
	void command(uint8_t data);
	virtual void write(uint8_t ch);
	void inc_row_column(void);
	uint8_t dc;
	uint8_t cs;
	uint8_t reset;
	uint8_t hardware_spi_num;
	uint8_t sdin;
	uint8_t sclk;
	uint8_t current_row, current_column;
};

#define LCD_LINES 6
#define LCD_COLS  14
#define LCD_WIDTH  84
#define LCD_HEIGHT 48


#endif
