#ifndef GUI_H
#define GUI_H

#include <xc.h>
#include <stdint.h>
#include "../state/state.h"

// GUI function prototypes
void gui_menu_handler(GameState *game); // Main menu state handler
void gui_test_handler(GameState *game); // Test mode state handler
void gui_simon_handler(GameState *game); // Simon Says state handler (placeholder)

#endif // GUI_H