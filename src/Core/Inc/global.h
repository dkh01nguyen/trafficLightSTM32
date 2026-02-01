/*
 * global.h
 * Global definitions, constants, and variables for traffic light system
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "stm32f1xx_hal.h"

// FSM States
typedef enum {
    STATE_INIT,       // Initialization state
    STATE_AUTO_NORM,  // Auto operation with normal mode
    STATE_AUTO_RED,   // Auto config RED mode
    STATE_AUTO_YEL,   // Auto config YELLOW mode
    STATE_AUTO_GRN,   // Auto config GREEN mode
    STATE_MANUAL,     // Manual operation mode
    STATE_MANUAL_FLASH_YEL, // Manual flash yellow
    STATE_MANUAL_FLASH_RED  // Manual flash red
} SystemState;

// Manual sub-states
typedef enum {
    MANUAL_NS_RED_EW_GREEN,  // NS: Red, EW: Green
    MANUAL_NS_GREEN_EW_RED   // NS: Green, EW: Red
} ManualSubState;

// Traffic light colors
typedef enum {
    LIGHT_OFF = 0b00,
    LIGHT_GREEN = 0b01,
    LIGHT_YELLOW = 0b10,
    LIGHT_RED = 0b11
} LightColor;

// Traffic light phases (for AUTO NORM mode)
typedef enum {
    PHASE_NS_GREEN_EW_RED,   // NS: Green, EW: Red
    PHASE_NS_YELLOW_EW_RED,  // NS: Yellow, EW: Red
    PHASE_NS_RED_EW_GREEN,   // NS: Red, EW: Green
    PHASE_NS_RED_EW_YELLOW   // NS: Red, EW: Yellow
} TrafficPhase;

// Global state variables
extern SystemState currentState;
extern ManualSubState manualSubState;
extern TrafficPhase currentPhase;

// Countdown times (in seconds)
extern uint8_t redDuration;
extern uint8_t yellowDuration;
extern uint8_t greenDuration;

// Current countdown timers
extern uint8_t nsCountdown;
extern uint8_t ewCountdown;

// Flash toggle flag
extern uint8_t flashToggle;

// Init display timer
extern uint8_t initDisplayCounter;

// Balance check flag
extern uint8_t isBalanced;

// Function prototypes
void global_init(void);
void save_durations_to_flash(void);
void load_durations_from_flash(void);

#endif /* INC_GLOBAL_H_ */
