#ifndef LCD_H
#define LCD_H
#define _XTAL_FREQ 4000000 

#define CURSOR_FORCE 0x80 // Force cursor position command

void pulse_enable(void); // Generate enable pulse
void push_Nibble(uint8_t nibble); // Push 4 bits to LCD
void push_byte(uint8_t byte); // Push 8 bits to LCD
void LCD_init(void); // Initialize LCD
void write_command(uint8_t command); // Write command to LCD
void write_data(uint8_t data); // Write data to LCD
void blink_cursor(void); // Blink cursor on LCD
void move_cursor(uint8_t row, uint8_t col); // Move cursor to specified position
void write_string(char* str, uint8_t row); // Write string to specified row
void GPIO_Init(void); // Initialize GPIO pins for LCD

#endif
