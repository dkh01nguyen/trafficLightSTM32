/*
 * fsm.c
 * Finite State Machine implementation for traffic light control
 * Handles all states, transitions, and display logic
 */

#include "fsm.h"
#include "global.h"
#include "button.h"
#include "light.h"
#include "i2c-lcd.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>

// Local variables
static uint8_t lcd_update_flag = 0;

/**
 * @brief Initialize FSM
 */
void fsm_init(void) {
    currentState = STATE_INIT;
    lcd_update_flag = 1;
}

/**
 * @brief Check if durations are balanced (R == Y + G)
 */
static uint8_t check_balance(void) {
    return (redDuration == (yellowDuration + greenDuration));
}

/**
 * @brief Update LCD display based on current state
 */
void fsm_lcd_update(void) {
    if (!lcd_update_flag) return;
    lcd_update_flag = 0;
    
    char line1[20] = {0};
    char line2[20] = {0};
    
    switch(currentState) {
        case STATE_INIT:
            sprintf(line1, "  HCMUT PROJECT  ");
            sprintf(line2, "TRAFFIC LIGHT  ");
            break;
            
        case STATE_AUTO_NORM:
            if (isBalanced) {
                // Display countdown for both directions based on current phase
                switch(currentPhase) {
                    case PHASE_NS_GREEN_EW_RED:
                        sprintf(line1, "  R:--  Y:--  G:%02d", nsCountdown);
                        sprintf(line2, "R:%02d  Y:--  G:--", ewCountdown);
                        break;
                    case PHASE_NS_YELLOW_EW_RED:
                        sprintf(line1, "  R:--  Y:%02d  G:--", nsCountdown);
                        sprintf(line2, "R:%02d  Y:--  G:--", ewCountdown);
                        break;
                    case PHASE_NS_RED_EW_GREEN:
                        sprintf(line1, "  R:%02d  Y:--  G:--", nsCountdown);
                        sprintf(line2, "R:--  Y:--  G:%02d", ewCountdown);
                        break;
                    case PHASE_NS_RED_EW_YELLOW:
                        sprintf(line1, "  R:%02d  Y:--  G:--", nsCountdown);
                        sprintf(line2, "R:--  Y:%02d  G:--", ewCountdown);
                        break;
                }
            } else {
                sprintf(line1, "  ERR: UNBALANCED");
                sprintf(line2, "R:%02d  Y:%02d  G:%02d", redDuration, yellowDuration, greenDuration);
            }
            break;
            
        case STATE_AUTO_RED:
            sprintf(line1, "  CONFIG RED     ");
            sprintf(line2, "R:%02d  Y:%02d  G:%02d", redDuration, yellowDuration, greenDuration);
            break;
            
        case STATE_AUTO_YEL:
            sprintf(line1, "  CONFIG YELLOW  ");
            sprintf(line2, "R:%02d  Y:%02d  G:%02d", redDuration, yellowDuration, greenDuration);
            break;
            
        case STATE_AUTO_GRN:
            sprintf(line1, "  CONFIG GREEN   ");
            sprintf(line2, "R:%02d  Y:%02d  G:%02d", redDuration, yellowDuration, greenDuration);
            break;
            
        case STATE_MANUAL:
            sprintf(line1, "  OPR: MANUAL    ");
            if (manualSubState == MANUAL_NS_RED_EW_GREEN) {
                sprintf(line2, "NS:R  EW:G     ");
            } else {
                sprintf(line2, "NS:G  EW:R     ");
            }
            break;
            
        case STATE_MANUAL_FLASH_YEL:
            sprintf(line1, "  OPR: MANUAL    ");
            sprintf(line2, "FLASH YEL      ");
            break;
            
        case STATE_MANUAL_FLASH_RED:
            sprintf(line1, "  OPR: MANUAL    ");
            sprintf(line2, "FLASH RED      ");
            break;
    }
    
    lcd_display_2lines(line1, line2);
}

/**
 * @brief Scan buttons and handle state transitions
 */
void fsm_button_scan(void) {
    // BUTTON_1_MOD1: Switch between AUTO and MANUAL modes
    if (is_button_pressed(BUTTON_1_MOD1)) {
        if (currentState == STATE_MANUAL || 
            currentState == STATE_MANUAL_FLASH_YEL || 
            currentState == STATE_MANUAL_FLASH_RED) {
            // Switch to AUTO mode (NORM with last saved durations)
            currentState = STATE_AUTO_NORM;
            manualSubState = MANUAL_NS_RED_EW_GREEN;
            currentPhase = PHASE_NS_GREEN_EW_RED;
            isBalanced = check_balance();
            
            if (isBalanced) {
                // Start normal operation
                nsCountdown = greenDuration;
                ewCountdown = redDuration;
                light_set_ns(LIGHT_GREEN);
                light_set_ew(LIGHT_RED);
            } else {
                // Show error
                nsCountdown = 0;
                ewCountdown = 0;
                light_off_all();
            }
        } else {
            // Switch to MANUAL mode (default state)
            currentState = STATE_MANUAL;
            manualSubState = MANUAL_NS_RED_EW_GREEN;
            light_set_ns(LIGHT_RED);
            light_set_ew(LIGHT_GREEN);
        }
        lcd_update_flag = 1;
    }
    
    // BUTTON_2_MOD1: Different behavior based on mode
    if (is_button_pressed(BUTTON_2_MOD1)) {
        if (currentState >= STATE_AUTO_NORM && currentState <= STATE_AUTO_GRN) {
            // AUTO mode: cycle through config modes
            switch(currentState) {
                case STATE_AUTO_NORM:
                    currentState = STATE_AUTO_RED;
                    light_off_all();
                    break;
                case STATE_AUTO_RED:
                    currentState = STATE_AUTO_YEL;
                    break;
                case STATE_AUTO_YEL:
                    currentState = STATE_AUTO_GRN;
                    break;
                case STATE_AUTO_GRN:
                    currentState = STATE_AUTO_NORM;
                    isBalanced = check_balance();
                    if (isBalanced) {
                        currentPhase = PHASE_NS_GREEN_EW_RED;
                        nsCountdown = greenDuration;
                        ewCountdown = redDuration;
                        light_set_ns(LIGHT_GREEN);
                        light_set_ew(LIGHT_RED);
                    } else {
                        light_off_all();
                    }
                    break;
                default:
                    break;
            }
            lcd_update_flag = 1;
        } else if (currentState == STATE_MANUAL) {
            // MANUAL mode: switch manual state
            if (manualSubState == MANUAL_NS_RED_EW_GREEN) {
                manualSubState = MANUAL_NS_GREEN_EW_RED;
                light_set_ns(LIGHT_GREEN);
                light_set_ew(LIGHT_RED);
            } else {
                manualSubState = MANUAL_NS_RED_EW_GREEN;
                light_set_ns(LIGHT_RED);
                light_set_ew(LIGHT_GREEN);
            }
            lcd_update_flag = 1;
        }
    }
    
    // BUTTON_1_MOD2: Different behavior based on mode
    if (is_button_pressed(BUTTON_1_MOD2)) {
        if (currentState == STATE_AUTO_RED) {
            // Increase RED duration
            redDuration++;
            if (redDuration > 99) redDuration = 1;
            lcd_update_flag = 1;
        } else if (currentState == STATE_AUTO_YEL) {
            // Increase YELLOW duration
            yellowDuration++;
            if (yellowDuration > 99) yellowDuration = 1;
            lcd_update_flag = 1;
        } else if (currentState == STATE_AUTO_GRN) {
            // Increase GREEN duration
            greenDuration++;
            if (greenDuration > 99) greenDuration = 1;
            lcd_update_flag = 1;
        } else if (currentState == STATE_MANUAL) {
            // Flash YELLOW
            currentState = STATE_MANUAL_FLASH_YEL;
            flashToggle = 0;
            lcd_update_flag = 1;
        } else if (currentState == STATE_MANUAL_FLASH_YEL) {
            // Return to default manual state
            currentState = STATE_MANUAL;
            manualSubState = MANUAL_NS_RED_EW_GREEN;
            light_set_ns(LIGHT_RED);
            light_set_ew(LIGHT_GREEN);
            lcd_update_flag = 1;
        }
    }
    
    // BUTTON_2_MOD2: Different behavior based on mode
    if (is_button_pressed(BUTTON_2_MOD2)) {
        if (currentState == STATE_AUTO_RED) {
            // Decrease RED duration
            redDuration--;
            if (redDuration < 1) redDuration = 99;
            lcd_update_flag = 1;
        } else if (currentState == STATE_AUTO_YEL) {
            // Decrease YELLOW duration
            yellowDuration--;
            if (yellowDuration < 1) yellowDuration = 99;
            lcd_update_flag = 1;
        } else if (currentState == STATE_AUTO_GRN) {
            // Decrease GREEN duration
            greenDuration--;
            if (greenDuration < 1) greenDuration = 99;
            lcd_update_flag = 1;
        } else if (currentState == STATE_MANUAL) {
            // Flash RED
            currentState = STATE_MANUAL_FLASH_RED;
            flashToggle = 0;
            lcd_update_flag = 1;
        } else if (currentState == STATE_MANUAL_FLASH_RED) {
            // Return to default manual state
            currentState = STATE_MANUAL;
            manualSubState = MANUAL_NS_RED_EW_GREEN;
            light_set_ns(LIGHT_RED);
            light_set_ew(LIGHT_GREEN);
            lcd_update_flag = 1;
        }
    }
}

/**
 * @brief Update countdown timers (called every 1 second)
 */
void fsm_countdown_update(void) {
    if (currentState != STATE_AUTO_NORM || !isBalanced) return;
    
    // Decrement countdowns
    if (nsCountdown > 0) nsCountdown--;
    if (ewCountdown > 0) ewCountdown--;
    
    // Check for phase transitions
    if (nsCountdown == 0 || ewCountdown == 0) {
        switch(currentPhase) {
            case PHASE_NS_GREEN_EW_RED:
                // NS: Green -> Yellow
                currentPhase = PHASE_NS_YELLOW_EW_RED;
                nsCountdown = yellowDuration;
                ewCountdown = yellowDuration;
                light_set_ns(LIGHT_YELLOW);
                light_set_ew(LIGHT_RED);
                break;
                
            case PHASE_NS_YELLOW_EW_RED:
                // NS: Yellow -> Red, EW: Red -> Green
                currentPhase = PHASE_NS_RED_EW_GREEN;
                nsCountdown = redDuration;
                ewCountdown = greenDuration;
                light_set_ns(LIGHT_RED);
                light_set_ew(LIGHT_GREEN);
                break;
                
            case PHASE_NS_RED_EW_GREEN:
                // EW: Green -> Yellow
                currentPhase = PHASE_NS_RED_EW_YELLOW;
                nsCountdown = yellowDuration;
                ewCountdown = yellowDuration;
                light_set_ns(LIGHT_RED);
                light_set_ew(LIGHT_YELLOW);
                break;
                
            case PHASE_NS_RED_EW_YELLOW:
                // EW: Yellow -> Red, NS: Red -> Green
                currentPhase = PHASE_NS_GREEN_EW_RED;
                nsCountdown = greenDuration;
                ewCountdown = redDuration;
                light_set_ns(LIGHT_GREEN);
                light_set_ew(LIGHT_RED);
                break;
        }
    }
    
    lcd_update_flag = 1;
}

/**
 * @brief Update flashing lights (called every 500ms)
 */
void fsm_flash_update(void) {
    if (currentState == STATE_AUTO_NORM && !isBalanced) {
        // Flash yellow when unbalanced
        light_toggle_both(LIGHT_YELLOW);
    } else if (currentState == STATE_AUTO_RED) {
        // Flash red in RED config mode
        light_toggle_both(LIGHT_RED);
    } else if (currentState == STATE_AUTO_YEL) {
        // Flash yellow in YELLOW config mode
        light_toggle_both(LIGHT_YELLOW);
    } else if (currentState == STATE_AUTO_GRN) {
        // Flash green in GREEN config mode
        light_toggle_both(LIGHT_GREEN);
    } else if (currentState == STATE_MANUAL_FLASH_YEL) {
        // Flash yellow in manual mode
        light_toggle_both(LIGHT_YELLOW);
    } else if (currentState == STATE_MANUAL_FLASH_RED) {
        // Flash red in manual mode
        light_toggle_both(LIGHT_RED);
    }
}

/**
 * @brief Main FSM run function (called from scheduler)
 */
void fsm_run(void) {
    // Handle INIT state
    if (currentState == STATE_INIT) {
        if (initDisplayCounter >= 3) {
            // After 3 seconds, switch to AUTO NORM mode
            currentState = STATE_AUTO_NORM;
            isBalanced = check_balance();
            if (isBalanced) {
                currentPhase = PHASE_NS_GREEN_EW_RED;
                nsCountdown = greenDuration;
                ewCountdown = redDuration;
                light_set_ns(LIGHT_GREEN);
                light_set_ew(LIGHT_RED);
            } else {
                light_off_all();
            }
            lcd_update_flag = 1;
        } else {
            initDisplayCounter++;
        }
    }
}
