/*
 * sched.h
 * Task scheduler for cooperative multitasking
 */

#ifndef INC_SCHED_H_
#define INC_SCHED_H_

#include <stdint.h>

// Maximum number of tasks
#define SCH_MAX_TASKS 10

// Task structure
typedef struct {
    void (*pTask)(void);  // Pointer to the task function
    uint32_t Delay;       // Delay (ticks) until the task will run
    uint32_t Period;      // Period (ticks) between repeated runs
    uint8_t RunMe;        // Flag indicating task is ready to run
    uint32_t TaskID;      // Task identifier
} sTask;

// Function prototypes
void SCH_Init(void);
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);
void SCH_Update(void);
void SCH_Dispatch_Tasks(void);
uint8_t SCH_Delete_Task(uint32_t taskID);

#endif /* INC_SCHED_H_ */
