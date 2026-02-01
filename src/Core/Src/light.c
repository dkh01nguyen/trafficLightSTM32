/*
 * light.c
 * Traffic light control implementation
 * 
 * Pin assignments (2-bit control for each direction):
 * NS (North-South): PA6 (bit 0), PA7 (bit 1)
 * EW (East-West):   PA8 (bit 0), PA9 (bit 1)
 * 
 * Light encoding:
 * 00 - OFF
 * 01 - GREEN
 * 10 - YELLOW
 * 11 - RED
 */

#include "light.h"
#include "main.h"

// Toggle states for flashing
static uint8_t ns_toggle_state = 0;
static uint8_t ew_toggle_state = 0;

/**
 * @brief Initialize traffic light module
 */
void light_init(void) {
    // Turn off all lights
    light_off_all();
    ns_toggle_state = 0;
    ew_toggle_state = 0;
}

/**
 * @brief Set North-South traffic light color
 * @param color: Light color to set
 */
void light_set_ns(LightColor color) {
    // Set bit 0 (PA6)
    HAL_GPIO_WritePin(Ans_GPIO_Port, Ans_Pin, (color & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // Set bit 1 (PA7)
    HAL_GPIO_WritePin(Bns_GPIO_Port, Bns_Pin, (color & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    ns_toggle_state = 1;
}

/**
 * @brief Set East-West traffic light color
 * @param color: Light color to set
 */
void light_set_ew(LightColor color) {
    // Set bit 0 (PA8)
    HAL_GPIO_WritePin(Aew_GPIO_Port, Aew_Pin, (color & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // Set bit 1 (PA9)
    HAL_GPIO_WritePin(Bew_GPIO_Port, Bew_Pin, (color & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    ew_toggle_state = 1;
}

/**
 * @brief Set both traffic lights to the same color
 * @param color: Light color to set
 */
void light_set_both(LightColor color) {
    light_set_ns(color);
    light_set_ew(color);
}

/**
 * @brief Turn off all traffic lights
 */
void light_off_all(void) {
    light_set_ns(LIGHT_OFF);
    light_set_ew(LIGHT_OFF);
}

/**
 * @brief Toggle North-South traffic light (for flashing)
 * @param color: Light color to toggle with
 */
void light_toggle_ns(LightColor color) {
    if (ns_toggle_state) {
        light_set_ns(LIGHT_OFF);
        ns_toggle_state = 0;
    } else {
        light_set_ns(color);
        ns_toggle_state = 1;
    }
}

/**
 * @brief Toggle East-West traffic light (for flashing)
 * @param color: Light color to toggle with
 */
void light_toggle_ew(LightColor color) {
    if (ew_toggle_state) {
        light_set_ew(LIGHT_OFF);
        ew_toggle_state = 0;
    } else {
        light_set_ew(color);
        ew_toggle_state = 1;
    }
}

/**
 * @brief Toggle both traffic lights (for flashing)
 * @param color: Light color to toggle with
 */
void light_toggle_both(LightColor color) {
    if (ns_toggle_state || ew_toggle_state) {
        light_off_all();
        ns_toggle_state = 0;
        ew_toggle_state = 0;
    } else {
        light_set_both(color);
        ns_toggle_state = 1;
        ew_toggle_state = 1;
    }
}
