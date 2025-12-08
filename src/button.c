#include "button.h"
#include "hardware.h"

void buttons_init(void) {
    ANSELE &= ~BUTTON; // set RE0 as digital
    TRISE |= BUTTON; // set RE0 as input
    WPUE |= BUTTON; // enable pull-up resistor on RE0
}

char read_button1(void) { // holy ? operator
    return (PORTE & BUTTON) ? 0 : 1;  
}