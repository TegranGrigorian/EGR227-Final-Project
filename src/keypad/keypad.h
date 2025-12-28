#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>

typedef struct { // keypad structure
    uint8_t cols;
    uint8_t rows;
    uint8_t pressed;
} keypad;

// Function prototypes
void init_keypad(void);
keypad numpad_press(void);
unsigned char numpad_value(uint8_t cols, uint8_t rows);
#endif
