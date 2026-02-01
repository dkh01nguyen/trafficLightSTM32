/*
 * timer.c
 * Timer interrupt handling implementation
 * TIM2 is configured to trigger every 10ms
 */

#include "timer.h"
#include "sched.h"
#include "button.h"

// Timer flags
uint8_t timer_flag_10ms = 0;
uint8_t timer_flag_1s = 0;

// Timer counter for 1 second (100 * 10ms = 1s)
uint16_t timer_counter_1s = 0;

/**
 * @brief Initialize timer variables
 */
void timer_init(void) {
    timer_flag_10ms = 0;
    timer_flag_1s = 0;
    timer_counter_1s = 0;
}

/**
 * @brief Timer interrupt handler - called every 10ms
 * This function is called from HAL_TIM_PeriodElapsedCallback
 */
void timer_run(void) {
    // Set 10ms flag
    timer_flag_10ms = 1;
    
    // Update button states (debouncing)
    button_reading();
    
    // Run scheduler
    SCH_Update();
    
    // Count to 1 second
    timer_counter_1s++;
    if (timer_counter_1s >= 100) { // 100 * 10ms = 1s
        timer_counter_1s = 0;
        timer_flag_1s = 1;
    }
}

/**
 * @brief Set a timer flag after a duration
 * @param flag: Pointer to the timer flag
 * @param duration: Duration in timer ticks (10ms units)
 */
void setTimer(uint8_t* flag, uint16_t duration) {
    *flag = duration;
}
