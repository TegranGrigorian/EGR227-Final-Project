# Project File Organization

## Directory Structure

```
├── button/
├── gui/
├── keypad/
├── lcd/
├── led/
├── servo/
├── simon/
├── speaker/
├── state/
├── test/
└── timer/
├── hardware.h
└── main.c
```

## Module Descriptions

### **button**
Handles initialization and processing of the fire button input. Provides debouncing logic and input detection to trigger game state transitions and gameplay mechanics.

### **gui**
Manages the graphical user interface state machine, including menu navigation and visual feedback. Handles state output logic and transitions between different GUI screens (splash screen, main menu, game over menu).

### **hardware.h**
Central repository for all hardware pin definitions and peripheral configurations. Provides a single source of truth for GPIO pin mappings, timer channels, PWM outputs, and device addresses used throughout the project.

### **keypad**
Implements keypad initialization and input processing for the 4x4 matrix keypad. Scans rows and columns, debounces inputs, and stores detected key presses in a structured format for game logic consumption.

### **lcd**
Provides comprehensive LCD control functions for a 16x2 character display. Includes initialization, data transmission, cursor positioning, and string writing capabilities. Manages timing delays required by HD44780 controller protocol.

### **led**
Handles RGB LED initialization and control. Includes functions to set LED colors, implement visual modes (GREEN_RED_YELLOW, YELLOW_RED_GREEN, RANDOM_STATIC), and update display patterns during gameplay. Defines LED enumeration types for mode selection.

### **main.c**
Contains the primary state machine orchestrator that coordinates the entire program flow. Initializes all peripherals and executes the main game loop, dispatching to appropriate handlers based on current program state.

### **servo**
Manages servo motor initialization and position control via PWM. Stores an array of predefined servo positions (POS_1 through POS_5) and provides functions to update duty cycles, enabling dynamic servo movement during gameplay.

### **simon**
Implements the Simon Says memory game logic. Contains initialization code to reset game state, multiple state handlers for game phases (SHOWING_SEQUENCE, WAITING_INPUT, SUCCESS, FAILURE), and a holistic game orchestrator that manages the complete Simon Says gameplay loop.

### **speaker**
Handles speaker initialization and audio control via PWM frequency modulation. Provides functions to generate sound effects and tones of varying frequencies and durations, used by both the target game and Simon Says game for audio feedback.

### **state**
Implements the target game state machine with four primary states (INIT, IDLE, PLAYING, GAME_OVER). Includes state handler functions that manage game initialization, idle waiting, active gameplay, and score display. Coordinates interactions with LED, servo, speaker, keypad, and LCD modules.

### **test**
Contains hardware diagnostic and testing functions that verify proper operation of all peripheral devices including buttons, keypads, LEDs, servos, speakers, and LCD displays. Used for development debugging and hardware validation.

### **timer**
Manages timer initialization and interrupt handling. Implements Timer 0 for precise 10 ms interrupt generation (100 interrupts per second), Timer 1 for blocking delays, and Timer 2/4 for PWM control. The Timer 0 ISR enforces the 30-second game duration by counting elapsed seconds.