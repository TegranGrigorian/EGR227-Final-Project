#ifndef SIMON_H
#define SIMON_H

#include <xc.h>
#include <stdint.h>
#include "../state/state.h"
#include "../servo/servo.h"

// Simon Says game constants
#define SIMON_MAX_SEQUENCE 20
#define SIMON_INITIAL_LENGTH 3
#define SIMON_LED_ON_TIME 500
#define SIMON_LED_OFF_TIME 250

// Simon Says game states
typedef enum {
    SIMON_SHOWING_SEQUENCE, // Displaying the sequence to memorize
    SIMON_WAITING_INPUT,  // Waiting for player input
    SIMON_SUCCESS, // Player completed sequence correctly
    SIMON_FAILURE, // Player got it wrong
    SIMON_GAME_OVER // Game ended
} SimonState;

// Simon Says game data structure
typedef struct {
    uint8_t sequence[SIMON_MAX_SEQUENCE]; // The LED sequence (0=Green, 1=Red, 2=Yellow)
    uint8_t fake_index; // Which index in sequence is the fake (beep) LED
    uint8_t sequence_length; // Current sequence length
    uint8_t current_step; // Current step in showing/input
    uint8_t player_step; // Player's current input step
    uint8_t score; // Player's score (rounds completed)
    SimonState simon_state; // Current simon game state
    uint8_t sequence_shown; // Flag: has sequence been shown this round?
} SimonGame;

// Function prototypes
void simon_init(SimonGame *simon);
void simon_generate_sequence(SimonGame *simon);
void simon_show_sequence(SimonGame *simon);
void simon_handle_input(SimonGame *simon, GameState *game);
uint8_t simon_check_input(SimonGame *simon, uint8_t led_aimed);
void simon_success_feedback(void);
void simon_failure_feedback(void);
void simon_handler(SimonGame *simon, GameState *game);

#endif // SIMON_H