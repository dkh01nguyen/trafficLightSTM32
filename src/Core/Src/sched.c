/*
 * sched.c
 * Task scheduler implementation
 * Cooperative multitasking scheduler with fixed time-slice execution
 */

#include "sched.h"
#include <string.h>

// Task array
static sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint32_t taskIDCounter = 0;

/**
 * @brief Initialize the scheduler
 */
void SCH_Init(void) {
    uint8_t i;
    for (i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask = 0;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;
        SCH_tasks_G[i].TaskID = 0;
    }
    taskIDCounter = 0;
}

/**
 * @brief Add a task to the scheduler
 * @param pFunction: Pointer to the task function
 * @param DELAY: Initial delay before first execution (in ticks)
 * @param PERIOD: Period between repeated executions (in ticks, 0 for one-shot)
 * @return Task ID (0 if failed)
 */
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    uint8_t Index = 0;
    
    // Find an empty slot
    while ((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS)) {
        Index++;
    }
    
    // If no empty slot found
    if (Index == SCH_MAX_TASKS) {
        return 0; // Failed to add task
    }
    
    // Add task to the array
    SCH_tasks_G[Index].pTask = pFunction;
    SCH_tasks_G[Index].Delay = DELAY;
    SCH_tasks_G[Index].Period = PERIOD;
    SCH_tasks_G[Index].RunMe = 0;
    
    // Assign unique task ID
    taskIDCounter++;
    SCH_tasks_G[Index].TaskID = taskIDCounter;
    
    return taskIDCounter;
}

/**
 * @brief Update the scheduler - called from timer ISR
 * Decrements delays and sets RunMe flag when delay reaches 0
 */
void SCH_Update(void) {
    uint8_t Index;
    
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
        // Check if there is a task at this location
        if (SCH_tasks_G[Index].pTask) {
            if (SCH_tasks_G[Index].Delay > 0) {
                // Decrement the delay
                SCH_tasks_G[Index].Delay--;
            } else {
                // Task is ready to run
                SCH_tasks_G[Index].RunMe = 1;
                
                // Reset delay for periodic tasks
                if (SCH_tasks_G[Index].Period > 0) {
                    SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period;
                }
            }
        }
    }
}

/**
 * @brief Dispatch tasks - call from main loop
 * Executes tasks that are ready to run
 */
void SCH_Dispatch_Tasks(void) {
    uint8_t Index;
    
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
        if (SCH_tasks_G[Index].RunMe > 0) {
            // Run the task
            (*SCH_tasks_G[Index].pTask)();
            
            // Reset RunMe flag
            SCH_tasks_G[Index].RunMe = 0;
            
            // If one-shot task, delete it
            if (SCH_tasks_G[Index].Period == 0) {
                SCH_Delete_Task(SCH_tasks_G[Index].TaskID);
            }
        }
    }
}

/**
 * @brief Delete a task from the scheduler
 * @param taskID: ID of the task to delete
 * @return 1 if successful, 0 if failed
 */
uint8_t SCH_Delete_Task(uint32_t taskID) {
    uint8_t Index;
    
    // Find the task with the given ID
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
        if (SCH_tasks_G[Index].TaskID == taskID) {
            // Delete the task
            SCH_tasks_G[Index].pTask = 0;
            SCH_tasks_G[Index].Delay = 0;
            SCH_tasks_G[Index].Period = 0;
            SCH_tasks_G[Index].RunMe = 0;
            SCH_tasks_G[Index].TaskID = 0;
            return 1; // Success
        }
    }
    
    return 0; // Task not found
}
