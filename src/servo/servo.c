#include "servo.h"
#include "hardware.h"

// Define constants
#define FOSC 4000000
#define TIMER_PRESCALER 7
#define T4_PERIOD_REG 155

// Servo position duty cycle mappings
const uint8_t SERVO_POSITIONS[5] = {42, 58, 75, 92, 108};
uint8_t current_duty_cycle = 75;  

void servo_init(void) { // port setup, timer4 setup, ccp2 setup for pwm, duty cycle init
    ANSELD &= ~SERVO;
    TRISD &= ~SERVO;
    LATD &= ~SERVO;
    T4CON = 0x00;
    T4TMR = 0x00;
    T4CLKCON = 0x01;
    T4PR = T4_PERIOD_REG;      
    T4CON = (TIMER_PRESCALER << 4);
    CCPTMRS0 &= ~0x0C;
    CCPTMRS0 |= 0x08;
    CCP2CON = 0x00;
    CCP2CON = 0x8C;
    RD7PPS = 0x0E;
    uint16_t ccpr_value = (((uint32_t)75 * 156) / 1000) << 2;
    CCPR2 = ccpr_value;
    current_duty_cycle = 75;
    T4CON |= 0x80;                       
}

int8_t servo_set_duty_cycle(uint8_t duty_cycle) { // set servo duty cycle, return -1 ifout of bounds
    if (duty_cycle < SERVO_MIN_DC || duty_cycle > SERVO_MAX_DC) {
        return -1;  
    }   
    uint16_t ccpr_value; // calculate CCPR2 value for desired duty cycle
    ccpr_value = (((uint32_t)duty_cycle * 156) / 1000) << 2; // ccpr_value = (duty_cycle% * 156) / 1000 * 4
    CCPR2 = ccpr_value;
    current_duty_cycle = duty_cycle;
    return 0;
}

uint8_t servo_get_duty_cycle(void) { // get current servo duty cycle
    return current_duty_cycle;
}

uint8_t clamp_servo_duty_cycle(uint8_t duty_cycle) { // clamp duty cycle to min/max limits
    if (duty_cycle < SERVO_MIN_DC) {
        return SERVO_MIN_DC;
    } else if (duty_cycle > SERVO_MAX_DC) {
        return SERVO_MAX_DC;
    } else {
        return duty_cycle;
    }
}

uint8_t servo_position_to_duty_cycle(ServoPosition pos) { // convert servo position enum to duty cycle
    return SERVO_POSITIONS[pos];
}