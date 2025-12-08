#ifndef SPEAKER_H
#define SPEAKER_H

#include <xc.h>

// Function prototypes for audio control
void init_speaker(void); // initialize speaker hardware
void play_fire_tone(void); // High frequency fire sound
void play_hit_tone(void); // High frequency success sound
void play_miss_tone(void); // Low frequency failure sound
void speaker_off(void); // i hate this spearer

#endif // SPEAKER_H
