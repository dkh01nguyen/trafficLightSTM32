/*
 * button.c
 * Button handling implementation with debouncing
 * Supports 4 buttons with press detection
 */

#include "button.h"
#include "main.h"

// Button GPIO pins (will be defined in main.h after GPIO config)
// Using these pins (avoiding already-in-use pins):
// BUTTON_1_MOD1: PB8
// BUTTON_2_MOD1: PB9
// BUTTON_1_MOD2: PB10
// BUTTON_2_MOD2: PB11

// Button state arrays
static uint8_t button_current[NUM_BUTTONS] = {BUTTON_RELEASED};
static uint8_t button_last[NUM_BUTTONS] = {BUTTON_RELEASED};
static uint8_t button_stable[NUM_BUTTONS] = {BUTTON_RELEASED};
static uint8_t button_debounce_counter[NUM_BUTTONS] = {0};

// Button press flags (set when button is pressed, cleared after read)
uint8_t button_flag[NUM_BUTTONS] = {0};

// Long press counters (in 10ms units, 100 = 1 second)
static uint16_t button_long_press_counter[NUM_BUTTONS] = {0};
static uint8_t button_long_press_flag[NUM_BUTTONS] = {0};

/**
 * @brief Initialize button module
 */
void button_init(void) {
    uint8_t i;
    for (i = 0; i < NUM_BUTTONS; i++) {
        button_current[i] = BUTTON_RELEASED;
        button_last[i] = BUTTON_RELEASED;
        button_stable[i] = BUTTON_RELEASED;
        button_debounce_counter[i] = 0;
        button_flag[i] = 0;
        button_long_press_counter[i] = 0;
        button_long_press_flag[i] = 0;
    }
}

/**
 * @brief Read button state from GPIO
 * @param index: Button index
 * @return GPIO state (inverted for pull-up buttons)
 */
static uint8_t read_button_gpio(uint8_t index) {
    GPIO_PinState state;
    
    switch(index) {
        case BUTTON_1_MOD1:
            state = HAL_GPIO_ReadPin(Button_1_GPIO_Port, Button_1_Pin);
            break;
        case BUTTON_2_MOD1:
            state = HAL_GPIO_ReadPin(Button_2_GPIO_Port, Button_2_Pin);
            break;
        case BUTTON_1_MOD2:
            state = HAL_GPIO_ReadPin(Button_3_GPIO_Port, Button_3_Pin);
            break;
        case BUTTON_2_MOD2:
            state = HAL_GPIO_ReadPin(Button_4_GPIO_Port, Button_4_Pin);
            break;
        default:
            return BUTTON_RELEASED;
    }
    
    // Invert for pull-up buttons (pressed = LOW)
    return (state == GPIO_PIN_RESET) ? BUTTON_PRESSED : BUTTON_RELEASED;
}

/**
 * @brief Read and debounce all buttons - called from timer ISR every 10ms
 */
void button_reading(void) {
    uint8_t i;
    
    for (i = 0; i < NUM_BUTTONS; i++) {
        // Read current button state
        button_current[i] = read_button_gpio(i);
        
        // Debouncing logic
        if (button_current[i] == button_last[i]) {
            button_debounce_counter[i]++;
            
            if (button_debounce_counter[i] >= DEBOUNCE_TIME) {
                // Stable state detected
                button_debounce_counter[i] = DEBOUNCE_TIME;
                
                // Check for state change
                if (button_current[i] != button_stable[i]) {
                    button_stable[i] = button_current[i];
                    
                    // Rising edge (Pressed) detection
                    if (button_stable[i] == BUTTON_PRESSED) {
                        button_flag[i] = 1;
                        button_long_press_counter[i] = 0;
                    }
                }
                
                // Long press logic
                if (button_stable[i] == BUTTON_PRESSED) {
                    button_long_press_counter[i]++;
                    if (button_long_press_counter[i] >= 100 && !button_long_press_flag[i]) {
                        // Long press detected (1 second)
                        button_long_press_flag[i] = 1;
                    }
                } else {
                    button_long_press_counter[i] = 0;
                    button_long_press_flag[i] = 0;
                }
            }
        } else {
            // State changed, reset counter
            button_debounce_counter[i] = 0;
        }
        
        button_last[i] = button_current[i];
    }
}

/**
 * @brief Check if button was pressed (one-shot, clears flag after read)
 * @param index: Button index
 * @return 1 if button was pressed, 0 otherwise
 */
uint8_t is_button_pressed(uint8_t index) {
    if (index >= NUM_BUTTONS) return 0;
    
    if (button_flag[index] == 1) {
        button_flag[index] = 0;  // Clear flag
        return 1;
    }
    return 0;
}

/**
 * @brief Check if button is being long pressed
 * @param index: Button index
 * @return 1 if button is long pressed, 0 otherwise
 */
uint8_t is_button_long_pressed(uint8_t index) {
    if (index >= NUM_BUTTONS) return 0;
    
    if (button_long_press_flag[index] == 1) {
        button_long_press_flag[index] = 0;  // Clear flag
        return 1;
    }
    return 0;
}
