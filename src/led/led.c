#include "led.h"
#include "../hardware.h"
#include "../timer/timer.h"
#include <stdlib.h>

#define _XTAL_FREQ 4000000

uint8_t last_led_second = 255;

void init_onboard_led(void) { // debug led init
    ANSELC &= ~0x80;   
    TRISC &= ~0x80;    
    LATC &= ~0x80;     // Turn off debug LED
}

void reset_led_timer_tracker(void) { // reset led second tracker for new game
    last_led_second = 255;
}

void init_leds(void) { // Initialize LED pins
    // RED = RB0, GREEN = RB2 (left), YELLOW = RC1 (right)
    ANSELB &= ~RED_LED; // RB0 (Red/middle) - digital
    ANSELB &= ~GREEN_LED; // RB2 (Green/left) - digital
    ANSELC &= ~YELLOW_LED; // RC1 (Yellow/right) - digital
    TRISB &= ~RED_LED; // RB0 as output
    TRISB &= ~GREEN_LED; // RB2 as output
    TRISC &= ~YELLOW_LED; // RC1 as output
    LATB &= ~RED_LED; // Red off
    LATB &= ~GREEN_LED; // Green off
    LATC &= ~YELLOW_LED; // Yellow off
}

void all_leds_off(void) {
    LATB &= ~(RED_LED | GREEN_LED); // Red and Green off (RB0, RB2)
    LATC &= ~YELLOW_LED; // Yellow off (RC1)
}

void set_led(uint8_t led_num) {
    all_leds_off(); // Turn all LEDs off before setting the desired one
    switch(led_num) {
        case 0:  // Green (LEFT) - RB2
            LATB |= GREEN_LED;
            break; // Turn on Green LED
        case 1:  // Red (MIDDLE) - RB0
            LATB |= RED_LED;
            break; // Turn on Red LED
        case 2:  // Yellow (RIGHT) - RC1
            LATC |= YELLOW_LED;
            break; // Turn on Yellow LED
    }
}

void update_led_targets(GameState *game) {
    // Get current elapsed seconds from the game timer (interrupt-driven)
    uint8_t current_second = game_timer_get_seconds();
    
    // Make sure first LED shows immediately when game starts (second 0)
    if (game->current_led == 3 && game->led_sequence_step == 0) {
        last_led_second = 255;  // Reset tracker
        switch(game->led_mode) {
            case MODE_GREEN_RED_YELLOW:
                game->current_led = 0;  // Green first
                set_led(game->current_led);
                game->led_sequence_step = 1;
                break;
            case MODE_YELLOW_RED_GREEN:
                game->current_led = 2;  // Yellow first
                set_led(game->current_led);
                game->led_sequence_step = 1;
                break;
            case MODE_RANDOM_STATIC:
                game->current_led = rand() % 3;
                set_led(game->current_led);
                break;
        }
        last_led_second = current_second;
        return;
    }
    
    // Check if a new second has passed
    if (current_second != last_led_second) {
        last_led_second = current_second;
        
        switch(game->led_mode) {
            case MODE_GREEN_RED_YELLOW:
                all_leds_off();
                game->current_led = game->led_sequence_step % 3;
                set_led(game->current_led);
                game->led_sequence_step++;
                break;
                
            case MODE_YELLOW_RED_GREEN:
                all_leds_off();
                // Reverse order: Yellow(2), Red(1), Green(0)
                {
                    uint8_t seq[3] = {2, 1, 0};
                    game->current_led = seq[game->led_sequence_step % 3];
                }
                set_led(game->current_led);
                game->led_sequence_step++;
                break;
                
            case MODE_RANDOM_STATIC:
                // LED stays on until hit - no update needed
                if (game->current_led == 3) {
                    // Select a random LED once
                    game->current_led = rand() % 3;
                    set_led(game->current_led);
                }
                break;
        }
    }
}
