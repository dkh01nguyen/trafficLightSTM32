/*
 * timer.h
 * Timer interrupt handling for 10ms timer tick
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32f1xx_hal.h"

// Timer flags for different subsystems
extern uint8_t timer_flag_10ms;
extern uint8_t timer_flag_1s;

// Timer counter
extern uint16_t timer_counter_1s;

// Function prototypes
void timer_init(void);
void timer_run(void);
void setTimer(uint8_t* flag, uint16_t duration);

#endif /* INC_TIMER_H_ */
