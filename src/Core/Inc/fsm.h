/*
 * fsm.h
 * Finite State Machine for traffic light control
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

#include "stm32f1xx_hal.h"

// Function prototypes
void fsm_init(void);
void fsm_run(void);
void fsm_button_scan(void);
void fsm_countdown_update(void);
void fsm_lcd_update(void);
void fsm_flash_update(void);

#endif /* INC_FSM_H_ */
