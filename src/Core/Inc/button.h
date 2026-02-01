/*
 * button.h
 * Button handling with debouncing and press detection
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "stm32f1xx_hal.h"

// Button indices
#define BUTTON_1_MOD1  0  // Module 1, Button 1 (Mode switch)
#define BUTTON_2_MOD1  1  // Module 1, Button 2 (Config mode/Manual state switch)
#define BUTTON_1_MOD2  2  // Module 2, Button 1 (Increase/Flash Yellow)
#define BUTTON_2_MOD2  3  // Module 2, Button 2 (Decrease/Flash Red)

#define NUM_BUTTONS    4

// Button states
#define BUTTON_RELEASED  0
#define BUTTON_PRESSED   1

// Debounce settings
#define DEBOUNCE_TIME    5  // 50 cycles of 10ms = 500ms

// Button press flags
extern uint8_t button_flag[NUM_BUTTONS];

// Function prototypes
void button_init(void);
void button_reading(void);
uint8_t is_button_pressed(uint8_t index);
uint8_t is_button_long_pressed(uint8_t index);

#endif /* INC_BUTTON_H_ */
