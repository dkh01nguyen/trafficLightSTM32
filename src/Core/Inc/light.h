/*
 * light.h
 * Traffic light control module
 * Controls 2 traffic light modules (NS and EW) with 2-bit binary signals
 */

#ifndef INC_LIGHT_H_
#define INC_LIGHT_H_

#include "stm32f1xx_hal.h"
#include "global.h"

// Traffic light direction
typedef enum {
    DIR_NS = 0,  // North-South
    DIR_EW = 1   // East-West
} TrafficDirection;

// Function prototypes
void light_init(void);
void light_set_ns(LightColor color);
void light_set_ew(LightColor color);
void light_set_both(LightColor color);
void light_off_all(void);
void light_toggle_ns(LightColor color);
void light_toggle_ew(LightColor color);
void light_toggle_both(LightColor color);

#endif /* INC_LIGHT_H_ */
