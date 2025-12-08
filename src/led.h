#ifndef LED_H
#define LED_H

#include <xc.h>
#include <stdint.h>

// LED target modes
typedef enum {
    MODE_GREEN_RED_YELLOW, // LEDs light from Green->Red->Yellow
    MODE_YELLOW_RED_GREEN, // LEDs light from Yellow->Red->Green
    MODE_RANDOM_STATIC // One random LED
} LEDMode;

// Function prototypes for LED control
void init_onboard_led(void);
void init_leds(void); // Initialize LED pins
void set_led(uint8_t led_num); // Turn on specific LED (0=Green, 1=Red, 2=Yellow)
void all_leds_off(void); // Turn all LEDs off
void update_led_targets(GameState *game); // Update LED sequence based on game mode
void reset_led_timer_tracker(void); // Reset LED second tracker for new game

#endif // LED_H
