#include <xc.h>
#include "lcd.h"
#include "hardware.h"
#include "timer.h"

#define _XTAL_FREQ 4000000

void pulse_enable(void) {   
    LATF &= ~E;         
    __delay_us(10);            
    LATF |=  E;          
    __delay_us(10);             
    LATF &= ~E;             
    __delay_us(10);
}
 
void push_Nibble(uint8_t nibble) {
    // Clear DB pins (RF2-RF5)
    LATF &= ~(DB[0]|DB[1]|DB[2]|DB[3]);
    // Set nibble on DB pins - DB array is {0x04,0x08,0x10,0x20}
    if (nibble & 0x01) LATF |= DB[0];  // RF2
    if (nibble & 0x02) LATF |= DB[1];  // RF3
    if (nibble & 0x04) LATF |= DB[2];  // RF4
    if (nibble & 0x08) LATF |= DB[3];  // RF5
    pulse_enable();
}
 
void push_byte(uint8_t byte) {
    push_Nibble(byte >> 4); // push MSBs
    push_Nibble(byte & 0x0F); // push the LSBs
    __delay_us(100);
}
 
void LCD_init(void) {
   /* ****** Reset ****** */ 
   write_command(0x30);
   __delay_ms(10);
   write_command(0x30);
   __delay_ms(10);
   write_command(0x30);
   __delay_ms(10);
      /* ****** Initialization ****** */ 
   write_command(0x02); // Setting 4 bit mode
   __delay_ms(10);
   write_command(0x28); // 2 lines, 5x7 format
   __delay_ms(10);
   write_command(0x0F); // display ON, cursor ON and blinking
   __delay_ms(10);
   write_command(0x01); // clear display
   __delay_ms(10);
   write_command(0x06); // increment cursor
   __delay_ms(10);
}
 
void write_command(uint8_t command) { 
    LATF &= ~RS; // drive the RS bit (RF0) LOW 
    __delay_ms(20); // generate 20 ms delay      
    push_byte(command); // call the push_byte function 
}

void write_data(uint8_t data) {   
    LATF |= RS; // drive the RS bit (RF0) HIGH for data
    __delay_us(100); // small delay
    push_byte(data); // call the push_byte function 
    __delay_us(100); // small delay after
}

void blink_cursor(void) {
    write_command(0x0F);
    timer1_delay_ms(500);
    // Turn cursor OFF (Display ON, Cursor OFF, Blink OFF)
    write_command(0x0C);
    timer1_delay_ms(500);
}

void GPIO_Init(void) {
    ANSELF &= ~(RS|E|DB[0]|DB[1]|DB[2]|DB[3]);     
    TRISF  &= ~(RS|E|DB[0]|DB[1]|DB[2]|DB[3]);     
}

void move_cursor(uint8_t row, uint8_t col) { // row is 1 to 3, col is 1 to 16, I sohuld of done 0 to 15 but im an idiot
    uint8_t location = 0;
    switch (row) {
        case 1: // 0x00
            location = 0x00 + col - 1;    
            break;
        case 2: // 0x40 but i did - 1
            location = 0x3F + col; // 0100 0000 - 1 = 0011 1111 
            break;
        case 3: // 0x10
            location = 0x10 + col;
            break;
        case 4:
            location = 0x4F + col;
            break;
    }
    location |= CURSOR_FORCE;
    
    // Set RS = 0 for command mode
    LATF &= ~RS; // Clear RS bit (RF0)
    push_byte(location); // Send the byte directly
}

void write_string(char* str, uint8_t row) {
    uint8_t i = 0;
    // enable lcd instruction
    LATF &= ~RS;
    move_cursor(row, 1);
    __delay_ms(5);  // slow clanker
    
    // Write each character until we hit null terminator or max 16 chars
    while (str[i] != '\0' && i < 16) {
        write_data(str[i]); // it uses ascii :)
        i++;
        __delay_ms(1); // had a little issues with overwrites
    }
}
