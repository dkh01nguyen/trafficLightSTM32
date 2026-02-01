/*
 * global.c
 * Global variables implementation
 */

#include "global.h"

// Global state variables
SystemState currentState = STATE_INIT;
ManualSubState manualSubState = MANUAL_NS_RED_EW_GREEN;
TrafficPhase currentPhase = PHASE_NS_GREEN_EW_RED;

// Default countdown times (in seconds)
// Default: R:5, Y:2, G:3 (balanced)
uint8_t redDuration = 5;
uint8_t yellowDuration = 2;
uint8_t greenDuration = 3;

// Current countdown timers
uint8_t nsCountdown = 0;
uint8_t ewCountdown = 0;

// Flash toggle flag
uint8_t flashToggle = 0;

// Init display timer
uint8_t initDisplayCounter = 0;

// Balance check flag
uint8_t isBalanced = 1;

/**
 * @brief Initialize global variables
 */
void global_init(void) {
    currentState = STATE_INIT;
    manualSubState = MANUAL_NS_RED_EW_GREEN;
    currentPhase = PHASE_NS_GREEN_EW_RED;
    
    // Default balanced times
    redDuration = 5;
    yellowDuration = 2;
    greenDuration = 3;
    
    nsCountdown = 0;
    ewCountdown = 0;
    flashToggle = 0;
    initDisplayCounter = 0;
    isBalanced = 1;
}

/**
 * @brief Save durations to flash (placeholder - can implement EEPROM emulation if needed)
 */
void save_durations_to_flash(void) {
    // For now, durations are stored in RAM
    // Can implement flash/EEPROM storage if needed
}

/**
 * @brief Load durations from flash (placeholder)
 */
void load_durations_from_flash(void) {
    // For now, use default values
    // Can implement flash/EEPROM storage if needed
}
