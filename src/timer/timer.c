#include <xc.h>
#include "timer.h"
#include "../button/button.h"
#define _XTAL_FREQ 4000000

// variables, volitaile for int adn static so they dont get errors for being used outside this file
static volatile uint8_t elapsed_seconds = 0;
static volatile uint8_t is_running = 0;
static volatile uint8_t timer_flag = 0;

// timer 1 stuff
void timer1_delay_ms(unsigned int ms) {
    unsigned int overflows = ms / 4;
    for (unsigned int i = 0; i < overflows; i++) {
        T1CON = 0x00; // setup timer, start timer, setup interupt, wait for it to overflow
        T1CLK = 0x01;
        T1CON = 0x30;
        TMR1H = 0xFE;
        TMR1L = 0x0C;
        PIR3 &= ~(1 << 4);
        T1CON |= 0x01;      
        while ((PIR3 & (1 << 4)) == 0) {
            ;
        }
        T1CON &= ~0x01;         
        PIR3 &= ~(1 << 4);      
    }
}

void timer1_delay_us(unsigned int us) {
    for (unsigned int i = 0; i < us; i++) {
        T1CON = 0x00; // setup timer, start timer, setup interupt, wait for it to overflow
        T1CLK = 0x01;
        T1CON = 0x00;
        TMR1H = 0xFF;
        TMR1L = 0xFE;
        PIR3 &= ~(1 << 4);
        T1CON |= 0x01;
        while ((PIR3 & (1 << 4)) == 0) {
            ;
        }
        T1CON &= ~0x01;         
        PIR3 &= ~(1 << 4);      
    }
}

void game_timer_init(void) {
    T0CON0 = 0x00; // timer setup
    T0CON1 = 0x00;
    T0CON1 |= (4 << 4); // fosc / 4
    T0CON1 |= (3 << 0); // 1:8
    T0CON0 |= (1 << 4); // 16 bit mode
    
    // preload for 10ms at Fosc/4 with prescaler 1:8
    unsigned short preload = 0xFFFF - 1249;
    TMR0L = (unsigned char)(0x00FF & preload);
    TMR0H = (unsigned char)((0xFF00 & preload) >> 8);
    
    // initialize variables
    elapsed_seconds = 0;
    is_running = 0;
    timer_flag = 0;
    
    // enable interrupt
    PIR3 &= ~(1 << 7);      
    PIE3 |= (1 << 7);       
    IPR3 &= ~(1 << 7);      
    INTCON0 |= 0xE0;        
}

void game_timer_start(void) {
    // reset variables
    elapsed_seconds = 0;
    is_running = 1;
    timer_flag = 0;

    // preload for 10ms at Fosc/4 with prescaler 1:8
    unsigned short preload = 0xFFFF - 1249;
    TMR0L = (unsigned char)(0x00FF & preload);
    TMR0H = (unsigned char)((0xFF00 & preload) >> 8);
    
    // enable interrupt and start timer
    PIR3 &= ~(1 << 7);
    T0CON0 |= (1 << 7);
}

void game_timer_stop(void) { // stop timer
    is_running = 0;
    T0CON0 &= ~(1 << 7);
}

void game_timer_reset(void) { // reset timer variables, happens at end and strat of target game
    // reset variables
    elapsed_seconds = 0;
    timer_flag = 0;
    
    // preload for 10ms at Fosc/4 with prescaler 1:8
    unsigned short preload = 0xFFFF - 1249;
    TMR0L = (unsigned char)(0x00FF & preload);
    TMR0H = (unsigned char)((0xFF00 & preload) >> 8);
    
    // clear interrupt flag
    PIR3 &= ~(1 << 7);      
}

uint8_t game_timer_is_running(void) {
    return is_running; // return if timer is running
}

uint8_t game_timer_get_seconds(void) {
    return elapsed_seconds; // return elapsed seconds
}

void __interrupt(irq(TMR0)) Timer0_GameISR(void) { // interupt function
    static uint8_t tick_count = 0; // avoid overwritting variable of same names
    if (PIR3 & (1 << 7)) {      
        PIR3 &= ~(1 << 7);      
        // preload for 10ms at Fosc/4 with prescaler 1:8
        unsigned short preload = 0xFFFF - 1249;
        TMR0L = (unsigned char)(0x00FF & preload);
        TMR0H = (unsigned char)((0xFF00 & preload) >> 8);
        
        if (is_running) { // if timer is running, increment tick count
            tick_count++;
            if (tick_count >= 100) {
                tick_count = 0;
                if (elapsed_seconds < 30) { // keep counting up to 30
                    elapsed_seconds++;
                }
                if (elapsed_seconds >= 30) { // stop timer at 30 seconds
                    is_running = 0;
                    T0CON0 &= ~(1 << 7);  
                }
            }
        }
    }
}