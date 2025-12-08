#ifndef TIMER_H
#define TIMER_H

#include <xc.h>
#include <stdint.h>

// function prototypes
// Timer 1 delay functions
void timer1_delay_ms(unsigned int ms);
void timer1_delay_us(unsigned int us);

// Game timer functions
void game_timer_init(void);
void game_timer_start(void);
void game_timer_stop(void);
uint8_t game_timer_get_seconds(void);
void game_timer_reset(void);
uint8_t game_timer_is_running(void);

#endif