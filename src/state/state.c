#include "state.h"
#include "../hardware.h"
#include "../lcd/lcd.h"
#include "../servo/servo.h"
#include "../button/button.h"
#include "../keypad/keypad.h"
#include "../timer/timer.h"
#include "../speaker/speaker.h"
#include "../led/led.h"
#include "../test/test.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define _XTAL_FREQ 4000000

void init_game_state(GameState *game) { // initalize alll game strucutre variables
    game->current_state = STATE_MENU;  
    game->hits = 0;
    game->misses = 0;
    game->time_remaining = 30;
    game->servo_pos = POS_3;  
    game->led_mode = MODE_GREEN_RED_YELLOW;
    game->current_led = 3;  
    game->led_sequence_step = 0;
    game->led_timer = 0;
    game->prev_fire_button = read_button1();  
}

void reset_game_state(GameState *game) { // reset game state variables for new game
    game->hits = 0;
    game->misses = 0;
    game->time_remaining = 30;
    game->led_sequence_step = 0;
    game->led_timer = 0;
    game->current_led = 3;
    all_leds_off();
    select_random_led_mode(game);
}

void state_init_handler(GameState *game) { // initial game state handler
    static uint32_t init_timer = 0;
    static char screen_written = 0;  
    
    if (!screen_written) { // 3 second delay with the title screen :)
        write_command(0x01);  
        __delay_ms(10);
        write_string("  TARGET  FUN  ", 1);
        write_string("   EGR 227     ", 2);
        write_string("Tegran Grigorian", 4);
        screen_written = 1;
        init_timer = 0;  
        
        servo_set_duty_cycle(servo_position_to_duty_cycle(POS_3)); // make sure servo is centered and ready
    }
    
    init_timer++; // increment timer every ms
    if (init_timer % 125 == 0) {
        LATC ^= 0x80;  
    }
    speaker_off(); // the peizo is so annoying
    if (init_timer >= 3000) { // 3 seconds elapsed
        LATC &= ~0x80; /// debug led off
        write_command(0x01);  
        __delay_ms(10); // wait for LCD to clear
        write_string("HITS:0  TIME:30", 1);
        write_string("MISSES:0       ", 2);
        game->current_state = STATE_IDLE; // move to idle state
        screen_written = 0;
        init_timer = 0;
    }
}

void state_idle_handler(GameState *game) {
    static char servo_centered = 0;  
    keypad key = numpad_press();
    if (key.pressed) { // if pressed, check for #
        uint8_t key_num = numpad_value(key.cols, key.rows);
        if (key_num == 12) {  // if # is pressed go back to main menu
            servo_centered = 0;
            all_leds_off();
            game->current_state = STATE_MENU;
            return;
        }
    }
    LATC &= ~0x80; // flash debug led off
    speaker_off(); // just to make sure this piece of poop is off
    if (!servo_centered) { // if it isnt centered, (just in case) center it
        game->servo_pos = POS_3;  
        servo_set_duty_cycle(servo_position_to_duty_cycle(POS_3));
        servo_centered = 1;
    }
    
    char fire_button = read_button1();
    if (fire_button && !game->prev_fire_button) { // check if the user starts the game
        LATC |= 0x80;  // debug led on
        reset_game_state(game); // reset game variabels
        game->current_state = STATE_PLAYING; // move to playing state
        game->led_timer = 0;
        game->time_remaining = 30; // just in case
        game_timer_reset(); // reset and start game timer
        game_timer_start(); // start game timer
        reset_led_timer_tracker();  // reset led timer tracker
        servo_centered = 0; // center again
        game->prev_fire_button = fire_button; // wipe previous button state
        return;  
    }
    game->prev_fire_button = fire_button; // update previous button state
}

void state_playing_handler(GameState *game) {
    // vars
    uint8_t elapsed = game_timer_get_seconds();
    uint8_t remaining = (elapsed >= 30) ? 0 : (30 - elapsed);
    LATC &= ~0x80; // debug led off
    speaker_off(); // start with speaker off
    
    game->time_remaining = remaining; // update time remaining
    update_lcd_game_screen(game); // update the lcd to show that
    
    if (elapsed >= 30 || !game_timer_is_running()) { // time is up
        game->current_state = STATE_GAME_OVER; // bye
        game_timer_stop(); // everything resets
        all_leds_off();
        return; // lave function so main loop can move us to game over
    }

    // handle game components
    update_led_targets(game);
    handle_servo_movement(game);
    handle_fire_button(game);
}

void state_game_over_handler(GameState *game) {
    keypad key = numpad_press(); // check keypad for # again
    if (key.pressed) {
        uint8_t key_num = numpad_value(key.cols, key.rows);
        if (key_num == 12) {  
            all_leds_off();
            game->current_state = STATE_MENU;
            return;
        }
    }

    all_leds_off(); // everything off
    game->time_remaining = 0; // sometimes the LCD would show a 1, so to ensrue we stop and print that, lets force it to be 0
    update_lcd_game_screen(game); // update lcd one last time
    speaker_off(); // please be off

    char fire_button = read_button1();
    if (fire_button && !game->prev_fire_button) { // check fire button to see if user wants to play again
        game->current_state = STATE_IDLE;
        reset_game_state(game);
        update_lcd_game_screen(game);
        game->prev_fire_button = fire_button;
        return;  
    }
    game->prev_fire_button = fire_button; // update previous button state
}

void update_lcd_game_screen(GameState *game) {
    char line1[17]; // 16 chars + null
    char line2[17];
    
    // buffers for each line
    sprintf(line1, "HITS:%-2d TIME:%-2d ", game->hits, game->time_remaining);
    sprintf(line2, "MISSES:%-2d      ", game->misses);
    
    // write the buffers to the LCD
    write_string(line1, 1);
    write_string(line2, 2);
}

// Handle servo movement based on keypad input
void handle_servo_movement(GameState *game) {
    keypad key = numpad_press();
    if (key.pressed) { // if a press occured
        LATC |= 0x80; // debug led on
        uint8_t key_num = numpad_value(key.cols, key.rows); // get the value
        if (key_num == 12) { // # key to return to menu
            game_timer_stop();
            all_leds_off();
            game->current_state = STATE_MENU; // go back to main menu
            LATC &= ~0x80; // debug led off
            return; // let main loop handle state change
        }
        
        if (key_num == 4) { // left key
            if (game->servo_pos < POS_5) { // move left if not at limit
                game->servo_pos++;
                servo_set_duty_cycle(servo_position_to_duty_cycle(game->servo_pos));
            }
        }

        else if (key_num == 6) { // right key
            
            if (game->servo_pos > POS_1) { // move right if not at limit
                game->servo_pos--;
                servo_set_duty_cycle(servo_position_to_duty_cycle(game->servo_pos));
            }
        }
        LATC &= ~0x80; // debug led off
    }
}

void handle_fire_button(GameState *game) {
    char fire_button = read_button1();
    if (fire_button && !game->prev_fire_button) { // rising edge detected
        play_fire_tone(); // play pew pew sound
        if (check_hit(game)) { // do we have a hit or miss
            game->hits++; // increment hits
            play_hit_tone(); // play hit sound
            all_leds_off(); // turn off leds, jus tin case
            game->current_led = 3; // reset current led
            select_random_led_mode(game); // select new led mode
            game->led_timer = 0; // reset led timer
            game->led_sequence_step = 0; // reset led sequence step
        }
        else { // you suck
            game->misses++; // increment misses
            play_miss_tone(); // play miss sound
        }
        
        update_lcd_game_screen(game); // update lcd with new score
        game->prev_fire_button = fire_button; // contain previous button state
        return;  
    }
    game->prev_fire_button = fire_button; // contain previous button state
}

uint8_t check_hit(GameState *game) {
    uint8_t aimed_led = 3; // default to none
    switch (game->servo_pos) { // determine if the LED is aimed properly to check for a hit
        case POS_1:
            aimed_led = 2;  // green  
            break;
        case POS_3:
            aimed_led = 1;  
            break;
        case POS_5:
            aimed_led = 0; // yellow  
            break;
        default:
            aimed_led = 3;  
            break;
    }
    return (aimed_led == game->current_led && game->current_led != 3); // return 1 if hit, 0 if miss
}

void select_random_led_mode(GameState *game) { // select a random led mode for next round, seed = 42, in main
    game->led_mode = rand() % 3;
    game->led_sequence_step = 0; // reset led sequence step
    game->current_led = 3;  
}