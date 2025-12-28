#ifndef SERVO_H
#define SERVO_H

#include <xc.h>
#include <stdint.h>

// little under maxs to not cook the servo into pieces
#define SERVO_MIN_DC 24
#define SERVO_MAX_DC 124

// Servo positions
typedef enum {
    POS_1 = 0, // Aims at Green LED; 25 = 2.5% DC
    POS_2 = 1, // Between Green and Red; 50 = 5.0% DC
    POS_3 = 2, // Aims at Red LED; 75 = 7.5% DC
    POS_4 = 3, // Between Red and Yellow; 100 = 10.0% DC
    POS_5 = 4  // Aims at Yellow LED; 125 = 12.5% DC
} ServoPosition;

// Function prototypes
void servo_init(void);
int8_t servo_set_duty_cycle(uint8_t duty_cycle);
uint8_t servo_get_duty_cycle(void);
uint8_t clamp_servo_duty_cycle(uint8_t duty_cycle); // not used :)
uint8_t servo_position_to_duty_cycle(ServoPosition pos); // convert servo position enum to duty cycle value

#endif // SERVO_H
