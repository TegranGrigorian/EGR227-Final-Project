#include "test.h"
#include "hardware.h"
#include "servo.h"
#include "lcd.h"
#include "led.h"
#include "keypad.h"
#include "speaker.h"
#include "state.h"

#define _XTAL_FREQ 4000000

// Static variables for test state machine, dont want them to overwrite anything else
static uint8_t test_phase = 0;
static char phase_started = 0;

void test_reset(void) {
    test_phase = 0;
    phase_started = 0;
}

// Returns 1 if test is complete or user pressed #, 0 otherwise
uint8_t test_handler(GameState *game) {
    // Check for # key to return to menu at any point
    keypad key = numpad_press();
    if (key.pressed) {
        uint8_t key_num = numpad_value(key.cols, key.rows);
        if (key_num == 12) {  // # key
            test_reset();
            all_leds_off();
            return 1;  // Signal to return to menu
        }
        
        // Handle phase-specific keys
        if (test_phase == 0 && key_num == 4) {
            LATC |= 0x80;  // Onboard LED on
            servo_set_duty_cycle(108);  // Move left
            __delay_ms(300);
            LATC &= ~0x80;  // Onboard LED off
            test_phase = 1;
            phase_started = 0;
        }
        
        else if (test_phase == 1 && key_num == 6) {
            LATC |= 0x80;  // Onboard LED on
            servo_set_duty_cycle(42);  // Move right
            __delay_ms(300);
            LATC &= ~0x80;  // Onboard LED off
            test_phase = 2;
            phase_started = 0;
        }
    }
    
    // Phase 0: Display "Press 4: Left"
    if (test_phase == 0) {
        if (!phase_started) {
            write_command(0x01);
            __delay_ms(5);
            write_string("Press 4: Left  ", 1);
            write_string("servo movement ", 2);
            phase_started = 1;
        }
    }
    
    // Phase 1: Display "Press 6: Right"
    else if (test_phase == 1) {
        if (!phase_started) {
            write_command(0x01);
            __delay_ms(5);
            write_string("Press 6: Right ", 1);
            write_string("servo movement ", 2);
            phase_started = 1;
        }
    }

    // Phase 2: Blink LEDs
    else if (test_phase == 2) {
        if (!phase_started) {
            write_command(0x01);
            __delay_ms(5);
            write_string("LED Blink Test ", 1);
            write_string("               ", 2);
            
            // Center servo
            servo_set_duty_cycle(75);
            __delay_ms(200);
            
            // Blink all LEDs 3 times
            for (uint8_t i = 0; i < 3; i++) {
                LATB |= (RED_LED | GREEN_LED); // Red and Green ON (RB0, RB2)
                LATC |= YELLOW_LED; // Yellow ON (RC1)
                __delay_ms(200);
                LATB &= ~(RED_LED | GREEN_LED); // Red and Green OFF
                LATC &= ~YELLOW_LED; // Yellow OFF
                __delay_ms(200);
            }
            
            test_phase = 3;
            phase_started = 0;
        }
    }

    // Phase 3: Piezo beep
    else if (test_phase == 3) {
        if (!phase_started) {
            write_command(0x01);
            __delay_ms(5);
            write_string("Piezo Beep Test", 1);
            write_string("               ", 2);
            
            // Play a beep sequence
            play_hit_tone();
            __delay_ms(200);
            play_miss_tone();
            __delay_ms(200);
            play_fire_tone();
            
            phase_started = 1;
            test_phase = 4;
        }
    }
    // Phase 4: Done - return to menu
    else if (test_phase == 4) {
        write_command(0x01);
        __delay_ms(5);
        write_string("Test Complete! ", 1);
        write_string("Returning...   ", 2);
        __delay_ms(1000);
        
        // Reset for next time
        test_reset();
        return 1;  // Signal to return to menu
    }
    
    return 0;  // Test still in progress
}
