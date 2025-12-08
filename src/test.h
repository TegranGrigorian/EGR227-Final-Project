#ifndef TEST_H
#define TEST_H

#include <xc.h>
#include "state.h"

// Function prototypes for hardware testing
void test_reset(void); // Reset test state machine
uint8_t test_handler(GameState *game); // Non-blocking test handler, returns 1 when done

#endif // TEST_H
