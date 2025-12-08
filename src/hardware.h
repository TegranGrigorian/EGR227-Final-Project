#ifndef HARDWARE_H
#define HARDWARE_H

// LCD pins
#define RS 0x01 // RF0
#define E 0x02 // RF 1
#define DB (uint8_t[]) {0x04,0x08,0x10,0x20}

// Physical layout: GREEN (left) - RED (middle) - YELLOW (right)
// LEFT position uses RB2, MIDDLE uses RB0, RIGHT uses RC1
#define RED_LED 0x01       // RB0 (MIDDLE)
#define GREEN_LED 0x04     // RB2 (LEFT position)
#define YELLOW_LED 0x02    // RC1 (RIGHT position)

// Speaker (PWM)
#define SPEAKER 0x02  // RB1 - PWM on CCP1

// Keypad
#define COL_3 0x01 // RD0
#define COL_2 0x02 // RD1
#define COL_1 0x04 // RD2
#define ROW_4 0x08 // RD3
#define ROW_3 0x10 // RD4
#define ROW_2 0x20 // RD5
#define ROW_1 0x40 // RD6

// Servo
#define SERVO 0x80 // RD7

// Button
#define BUTTON 0x01 // RE0

#endif // HARDWARE_H
