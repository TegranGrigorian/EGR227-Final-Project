#include "keypad.h"
#include "hardware.h"
#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 4000000
#define ALL_ROWS 0x78  
#define ALL_COLS 0x07  

// vars, static since it only needs to stay in this file.
static uint8_t prev_key_pressed = 0;

void init_keypad(void) { // keypad port setup
    ANSELD &= ~(COL_1 | COL_2 | COL_3);
    TRISD &= ~(COL_1 | COL_2 | COL_3);
    LATD |= (COL_1 | COL_2 | COL_3);  
    ODCOND |= (COL_1 | COL_2 | COL_3); // open drain
    ANSELD &= ~(ROW_1 | ROW_2 | ROW_3 | ROW_4);
    TRISD |= (ROW_1 | ROW_2 | ROW_3 | ROW_4);
    WPUD |= (ROW_1 | ROW_2 | ROW_3 | ROW_4); // internal pull-ups
}

// Scans the keypad and returns which key is pressed
keypad numpad_press(void) {
    keypad result = {0, 0, 0};  
    uint8_t cols;
    uint8_t rows;
    uint8_t key_detected = 0;
    for (cols = 0; cols < 3; cols++) { // scan each column
        LATD |= ALL_COLS;               
        LATD &= ~(0x04 >> cols);        
        __delay_us(10); // small debounce delay
        rows = PORTD & ALL_ROWS; // read rows
        if (rows != ALL_ROWS) { // key press detected
            key_detected = 1;
            break;  
        }
    }
    // Reset columns to high after scanning
    LATD |= ALL_COLS;
    
    if (!key_detected) { // no key pressed
        prev_key_pressed = 0;
        return result;  
    }
    
    if (prev_key_pressed) { // key was already pressed
        return result;  
    }
    
    // Mark that a key is now pressed
    prev_key_pressed = 1;
    result.cols = cols;
    result.rows = rows;
    result.pressed = 1;
    return result;
}

// Converts column and row to keypad value (1-12)
unsigned char numpad_value(uint8_t cols, uint8_t rows) {    
    switch(rows) {
        case 0x38:  
            return cols + 1;
        case 0x58:  
            return cols + 4;
        case 0x68:  
            return cols + 7;
        case 0x70:  
            return cols + 10;
        default:
            return 0;  
    }
}