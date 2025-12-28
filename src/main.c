#include "hardware.h"
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "state/state.h"
#include "lcd/lcd.h"
#include "servo/servo.h"
#include "button/button.h"
#include "keypad/keypad.h"
#include "timer/timer.h"
#include "speaker/speaker.h"
#include "led/led.h"
#include "gui/gui.h"
#include "servo/servo.h"

#define _XTAL_FREQ 4000000
#pragma config WDTE = OFF       

void system_init(void) { // holy initalize
    GPIO_Init(); // gpio for the LCD 
    init_leds();
    init_speaker();
    init_onboard_led(); // debug led
    init_keypad();
    buttons_init();
    servo_init();
    LCD_init();
    game_timer_init(); // Timer0 for game timing
    srand(42);  // 42 is the answer to the univese
}

void main(void) {
    system_init();
    GameState game; // game state machine structure
    init_game_state(&game); // initalize the game state strucutre/vars
    
    // main loop
    while(1) {
        switch(game.current_state) { // state machine current state structure organizer
            case STATE_MENU: // main menu screen
                gui_menu_handler(&game);
                break;
                
            case STATE_INIT: // initial game state
                state_init_handler(&game);
                break;
                
            case STATE_IDLE: // idle waiting for fire button to go
                state_idle_handler(&game);
                break;
                
            case STATE_PLAYING: // pew pew
                state_playing_handler(&game);
                break;
                
            case STATE_GAME_OVER: // game over
                state_game_over_handler(&game);
                break;
                
            case STATE_TEST: //  test screen, option 3
                gui_test_handler(&game);
                break;
                
            case STATE_SIMON_SAYS: // Simon Says, option 2
                gui_simon_handler(&game);
                break;
                
            default: // just in case a oopsie occurs
                game.current_state = STATE_MENU;
                break;
        }
    }
}
