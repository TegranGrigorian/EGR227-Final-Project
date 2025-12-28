#include "gui.h"
#include "../hardware.h"
#include "../lcd/lcd.h"
#include "../servo/servo.h"
#include "../keypad/keypad.h"
#include "../speaker/speaker.h"
#include "../led/led.h"
#include "../state/state.h"
#include "../simon/simon.h"
#include "../test/test.h"

#define _XTAL_FREQ 4000000

// Simon Says game instance
static SimonGame simon_game;
static uint8_t simon_initialized = 0;

void gui_menu_handler(GameState *game) { // Main menu state handler
    static char menu_displayed = 0;
    
    // Display menu only once when entering this state
    if (!menu_displayed) {
        write_command(0x01);  // Clear display
        __delay_ms(10);
        write_string("1:Game 2:Simon", 1);
        write_string("3:Test         ", 2);
        write_string("#: Main Menu   ", 4);
        menu_displayed = 1;
        
        // Center servo and turn off LEDs
        servo_set_duty_cycle(servo_position_to_duty_cycle(POS_3));
        all_leds_off();
        LATC &= ~0x80;  // Onboard LED off
    }
    
    // Check keypad for selection
    keypad key = numpad_press();
    
    if (key.pressed) {
        uint8_t key_num = numpad_value(key.cols, key.rows);
        
        if (key_num == 1) { // Target Game selected
            menu_displayed = 0;
            game->current_state = STATE_INIT;
        }
        else if (key_num == 2) { // Simon Says selected
            menu_displayed = 0;
            game->current_state = STATE_SIMON_SAYS;
        }
        else if (key_num == 3) { // Test mode selected
            menu_displayed = 0;
            game->current_state = STATE_TEST;
        }
    }
}

void gui_test_handler(GameState *game) { // move to test state and handle output
    if (test_handler(game)) { // if test is complete or # pressed
        game->current_state = STATE_MENU;
    }
}

void gui_simon_handler(GameState *game) { // Simon Says state handler
    // Initialize Simon game on first entry
    if (!simon_initialized) {
        simon_init(&simon_game);
        simon_initialized = 1;
        
        // Center servo
        game->servo_pos = POS_3;
        servo_set_duty_cycle(servo_position_to_duty_cycle(POS_3));
    }
    
    // Run the Simon Says game handler
    simon_handler(&simon_game, game);
    
    if (game->current_state == STATE_MENU) { // if we are leaving simon says
        simon_initialized = 0;
    }
}