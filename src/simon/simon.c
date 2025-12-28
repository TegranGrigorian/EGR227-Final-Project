#include "simon.h"
#include "../hardware.h"
#include "../lcd/lcd.h"
#include "../led/led.h"
#include "../speaker/speaker.h"
#include "../servo/servo.h"
#include "../button/button.h"
#include "../keypad/keypad.h"
#include "state.h"
#include <stdlib.h>
#include <stdio.h>
#define _XTAL_FREQ 4000000

void simon_init(SimonGame *simon) { // initalize simon game structure
    simon->sequence_length = SIMON_INITIAL_LENGTH;
    simon->current_step = 0;
    simon->player_step = 0;
    simon->score = 0;
    simon->simon_state = SIMON_SHOWING_SEQUENCE;
    simon->fake_index = 255;  // No fake initially
    simon->sequence_shown = 0;  // Not shown yet
    
    // Generate initial sequence
    simon_generate_sequence(simon); // generate the first sequence
}

void simon_generate_sequence(SimonGame *simon) { // randomly generate simon sequence
    // Generate random sequence of LEDs (0=Green, 1=Red, 2=Yellow)
    for (uint8_t i = 0; i < simon->sequence_length; i++) {
        simon->sequence[i] = rand() % 3;
    }
    
    // Add a fake LED with beep (50% chance if sequence length > 2)
    if (simon->sequence_length > 2 && (rand() % 2) == 0) {
        // Pick a random position for the fake (not first or last)
        simon->fake_index = 1 + (rand() % (simon->sequence_length - 1));
        // Make sure fake LED is different from previous
        uint8_t prev_led = simon->sequence[simon->fake_index - 1];
        simon->sequence[simon->fake_index] = (prev_led + 1 + (rand() % 2)) % 3;
    } else {
        simon->fake_index = 255;  // No fake this round (lucky you)
    }
    
    // Reset for showing
    simon->current_step = 0;
    simon->player_step = 0;
    simon->sequence_shown = 0;  // Reset flag so sequence will be shown
}

void simon_show_led(uint8_t led, uint8_t is_fake) { // show a single LED in the sequence
    all_leds_off();
    set_led(led);
    
    if (is_fake) {
        // Play beep for fake LED
        play_fire_tone();
    }
    
    __delay_ms(SIMON_LED_ON_TIME); // just so they have time
    all_leds_off();
    __delay_ms(SIMON_LED_OFF_TIME);
}

void simon_show_sequence(SimonGame *simon) { // show the full simon sequence
    // Only show sequence once per round
    if (simon->sequence_shown) {
        return;
    }
    simon->sequence_shown = 1;  // Mark as shown
    
    // Display "Watch!" on LCD
    write_command(0x01);
    __delay_ms(5);
    write_string("   WATCH!      ", 1);
    char line2[17];
    sprintf(line2, "Score: %-3d     ", simon->score);
    write_string(line2, 2);
    
    __delay_ms(500);  // the average human reaction time is 200 ms fun little fact
    
    // Show each LED in sequence
    for (uint8_t i = 0; i < simon->sequence_length; i++) {
        uint8_t is_fake = (i == simon->fake_index);
        simon_show_led(simon->sequence[i], is_fake);
    }
    
    // Transition to waiting for input
    simon->simon_state = SIMON_WAITING_INPUT;
    simon->player_step = 0;
    
    // Update LCD for input phase
    write_command(0x01);
    __delay_ms(5);
    write_string("  YOUR TURN!   ", 1);
    sprintf(line2, "Step: 1/%-2d     ", simon->sequence_length - (simon->fake_index < simon->sequence_length ? 1 : 0));
    write_string(line2, 2);
}

uint8_t simon_get_aimed_led(GameState *game) {
    // LED positions: GREEN=left, RED=middle, YELLOW=right
    // Servo: POS_1=left(42), POS_3=middle(75), POS_5=right(108)
    if (game->servo_pos == POS_1) {
        return 2; // Green (LEFT)
    }
    else if (game->servo_pos == POS_3) {
        return 1; // Red (MIDDLE)
    }
    else if (game->servo_pos == POS_5) {
        return 0; // Yellow (RIGHT)
    }
    return 3; // No LED aimed
}

uint8_t simon_check_input(SimonGame *simon, uint8_t led_aimed) {
    // Find the expected LED (skip fake)
    uint8_t expected_step = simon->player_step;
    uint8_t sequence_index = 0;
    uint8_t real_step = 0;
    
    // Count through sequence, skipping fake
    for (uint8_t i = 0; i < simon->sequence_length; i++) {
        if (i == simon->fake_index) continue;  // Skip fake
        
        if (real_step == expected_step) { // Found the matching step
            sequence_index = i;
            break;
        }
        real_step++; // Increment real step count, skipping fake
    }
    return (led_aimed == simon->sequence[sequence_index]);
}

uint8_t simon_get_real_length(SimonGame *simon) {
    // Return sequence length minus fake if present
    if (simon->fake_index < simon->sequence_length) {
        return simon->sequence_length - 1;
    }
    return simon->sequence_length;
}

void simon_success_feedback(void) { // good job
    // All LEDs light up + success tone
    LATB |= (RED_LED | GREEN_LED); // Red and Green ON (RB0, RB2)
    LATC |= YELLOW_LED; // Yellow ON (RC1)
    play_hit_tone();
    __delay_ms(300);
    play_hit_tone();
    __delay_ms(500);
    all_leds_off();
}

void simon_failure_feedback(void) { // u suck
    // Red LED + failure tone
    all_leds_off();
    LATB |= RED_LED; // Red ON (RB0)
    play_miss_tone();
    __delay_ms(200);
    play_miss_tone();
    __delay_ms(200);
    play_miss_tone();
    __delay_ms(500);
    all_leds_off();
}

void simon_handle_input(SimonGame *simon, GameState *game) { // handle player input during simon says
    static uint8_t prev_fire = 0;
    char fire_button = read_button1();
    
    // Handle servo movement (same as target game)
    keypad key = numpad_press();
    if (key.pressed) {
        uint8_t key_num = numpad_value(key.cols, key.rows);
        
        // Check for # key to return to menu
        if (key_num == 12) {
            all_leds_off();
            game->current_state = STATE_MENU;
            return;
        }
        
        if (key_num == 4) { // left key
            if (game->servo_pos < POS_5) {
                game->servo_pos++;
                servo_set_duty_cycle(servo_position_to_duty_cycle(game->servo_pos));
            }
        }
        else if (key_num == 6) { // right key
            if (game->servo_pos > POS_1) {
                game->servo_pos--;
                servo_set_duty_cycle(servo_position_to_duty_cycle(game->servo_pos));
            }
        }
    }
    
    // Check fire button (rising edge)
    if (fire_button && !prev_fire) { // fire button pressed
        uint8_t aimed_led = simon_get_aimed_led(game);
        
        if (aimed_led != 3) {  // Valid LED aimed
            play_fire_tone();
            
            if (simon_check_input(simon, aimed_led)) { // check if input is correct
                simon->player_step++;
                
                // Flash the LED briefly to confirm
                set_led(aimed_led);
                __delay_ms(100);
                all_leds_off();
                
                uint8_t real_length = simon_get_real_length(simon);
                
                if (simon->player_step >= real_length) {
                    // Completed the sequence!
                    simon->simon_state = SIMON_SUCCESS;
                } else {
                    // Update LCD with progress
                    char line2[17];
                    sprintf(line2, "Step: %d/%-2d     ", simon->player_step + 1, real_length);
                    write_string(line2, 2);
                }
            } else {
                // your bad
                simon->simon_state = SIMON_FAILURE;
            }
        }
    }
    
    prev_fire = fire_button;
}

void simon_handler(SimonGame *simon, GameState *game) { // main simon says state machine handler
    switch (simon->simon_state) {
        case SIMON_SHOWING_SEQUENCE:
            simon_show_sequence(simon);
            break;
            
        case SIMON_WAITING_INPUT:
            simon_handle_input(simon, game);
            break;
            
        case SIMON_SUCCESS:
            simon_success_feedback();
            simon->score++;
            
            // Increase difficulty, havent beat this game yet
            if (simon->sequence_length < SIMON_MAX_SEQUENCE) {
                simon->sequence_length++;
            }
            
            // Generate new sequence and show it
            simon_generate_sequence(simon);
            simon->simon_state = SIMON_SHOWING_SEQUENCE;
            break;
            
        case SIMON_FAILURE:
            simon_failure_feedback();
            
            // Show game over screen
            write_command(0x01);
            __delay_ms(5);
            write_string("  GAME OVER!   ", 1);
            char line2[17];
            sprintf(line2, "Final Score: %-2d", simon->score);
            write_string(line2, 2);
            
            simon->simon_state = SIMON_GAME_OVER;
            break;
            
        case SIMON_GAME_OVER:
            // Wait for fire button to restart or go back to menu
            { // code blcok because I suck at making variables and I want these to be local :)
                static uint8_t prev_btn = 0;
                char fire_button = read_button1();
                
                if (fire_button && !prev_btn) {
                    // Restart Simon Says
                    simon_init(simon);
                }
                prev_btn = fire_button;
            }
            break; // leave
    }
}