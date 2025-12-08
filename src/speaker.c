#include <xc.h>
#include "speaker.h"
#include "hardware.h"
#define _XTAL_FREQ 4000000

void init_speaker(void) { // initalize pins, timer, ccp for pwm
    ANSELB &= ~SPEAKER;   
    TRISB &= ~SPEAKER;    
    LATB &= ~SPEAKER;     
    T2CON &= ~0x80;       
    T2PR = 155;           
    T2CLKCON = 0x01;      
    T2TMR = 0;            
    T2CON = 0x70;         
    T2CON |= 0x80;        
    CCP1CON &= ~0x80;     
    CCP1CON = 0x8C;       
    RB1PPS = 0x0D;        
    CCPR1 = 0;
}

void play_fire_tone(void) {
    uint16_t pwm_value = (156 / 2); // 5khz high pitch tone PEW  
    CCPR1 = pwm_value << 2;  
    __delay_ms(80);  
    CCPR1 = 0;  
}

void play_hit_tone(void) { // bad dong sound effect
    uint16_t pwm_value = ((156 * 9) / 10); // change duty cycle
    CCPR1 = pwm_value << 2;  
    __delay_ms(250);  
    CCPR1 = 0;  
}

void play_miss_tone(void) { // quiet low tone
    uint16_t pwm_value = ((156) / 10); // change duty cycle
    CCPR1 = pwm_value << 2;  
    __delay_ms(80);  
    CCPR1 = 0;  
}

void speaker_off(void) { // this peizo is SO ANNOYING
    CCPR1 = 0;  
}
