#ifndef STATE_H
#define STATE_H

#include <xc.h>
#include <stdint.h>
#include "servo.h"

// Define system states
typedef enum {
    STATE_MENU, // main menu
    STATE_INIT, // inital target game state
    STATE_IDLE, // waiting for fire button
    STATE_PLAYING, // game in progress
    STATE_GAME_OVER, // done
    STATE_SIMON_SAYS, // simon says
    STATE_TEST // test mode
} SystemState;

// Game state structure
typedef struct {
    SystemState current_state;
    uint8_t hits;
    uint8_t misses;
    uint8_t time_remaining;
    ServoPosition servo_pos;
    LEDMode led_mode;
    uint8_t current_led; // Which LED is currently lit (0=Green, 1=Red, 2=Yellow, 3=None)
    uint8_t led_sequence_step; // For sequential modes
    uint32_t led_timer; // Timer for LED transitions
    uint8_t prev_fire_button; // For button detection
} GameState;

// Function prototypes for state handlers
void state_init_handler(GameState *game);
void state_idle_handler(GameState *game);
void state_playing_handler(GameState *game);
void state_game_over_handler(GameState *game);

// Helper functions
void init_game_state(GameState *game);
void reset_game_state(GameState *game);
void update_lcd_game_screen(GameState *game);
void handle_servo_movement(GameState *game);
void handle_fire_button(GameState *game);
uint8_t check_hit(GameState *game);
void select_random_led_mode(GameState *game);

#endif // STATE_H
