/*
 * FreeRTOS Priority-Urgency Quadrant Model Demonstration - ENHANCED VERSION
 * Linux POSIX Port Version with CPU Stress
 *
 * This version adds CPU-intensive work to clearly demonstrate the difference
 * between PROPER and IMPROPER priority assignments
 *
 * Author: Azad Mohammed Shaik
 * Date: October 30, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>

#include "FreeRTOSConfig_linux.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/* ============================================================================
 * CONFIGURATION - Change this to see different scenarios
 * ============================================================================ */

// Set to 1 for PROPER priorities (as per quadrant model)
// Set to 0 for IMPROPER priorities (ad-hoc, all high priority)
#define USE_PROPER_PRIORITIES  1

// Set to 1 to enable CPU-intensive work (demonstrates priority issues)
#define USE_CPU_STRESS  1

/* ============================================================================
 * PRIORITY DEFINITIONS
 * ============================================================================ */

#if USE_PROPER_PRIORITIES
    // PROPER: Following Priority-Urgency Quadrant Model
    // Q1: High Urgency + High Criticality (Priorities 9-8)
    #define PRIORITY_SAFETY_CRITICAL    (configMAX_PRIORITIES - 1)  // 9
    #define PRIORITY_CAN_HANDLER        (configMAX_PRIORITIES - 2)  // 8
    #define PRIORITY_MOTOR_CONTROL      (configMAX_PRIORITIES - 2)  // 8
    #define PRIORITY_SENSOR_FUSION      (configMAX_PRIORITIES - 1)  // 9

    // Q2: Low Urgency + High Criticality (Priority 7)
    #define PRIORITY_DIAGNOSTICS        (configMAX_PRIORITIES - 3)  // 7
    #define PRIORITY_WATCHDOG           (configMAX_PRIORITIES - 3)  // 7
    #define PRIORITY_HEALTH_MONITOR     (configMAX_PRIORITIES - 3)  // 7
    #define PRIORITY_ERROR_HANDLER      (configMAX_PRIORITIES - 3)  // 7

    // Q3: High Urgency + Low Criticality (Priority 5)
    #define PRIORITY_UI_EVENTS          (configMAX_PRIORITIES - 5)  // 5
    #define PRIORITY_DISPLAY_REFRESH    (configMAX_PRIORITIES - 5)  // 5
    #define PRIORITY_BUTTON_HANDLER     (configMAX_PRIORITIES - 5)  // 5
    #define PRIORITY_ANIMATION          (configMAX_PRIORITIES - 5)  // 5

    // Q4: Low Urgency + Low Criticality (Priority 1)
    #define PRIORITY_DATA_LOGGING       (tskIDLE_PRIORITY + 1)      // 1
    #define PRIORITY_FILE_CLEANUP       (tskIDLE_PRIORITY + 1)      // 1
    #define PRIORITY_STATISTICS         (tskIDLE_PRIORITY + 1)      // 1
    #define PRIORITY_TELEMETRY          (tskIDLE_PRIORITY + 1)      // 1
#else
    // IMPROPER: Ad-hoc high priorities (common mistake)
    // Q1: High Urgency + High Criticality
    #define PRIORITY_SAFETY_CRITICAL    (configMAX_PRIORITIES - 1)  // 9
    #define PRIORITY_CAN_HANDLER        (configMAX_PRIORITIES - 1)  // 9
    #define PRIORITY_MOTOR_CONTROL      (configMAX_PRIORITIES - 1)  // 9
    #define PRIORITY_SENSOR_FUSION      (configMAX_PRIORITIES - 1)  // 9

    // Q2: Low Urgency + High Criticality - Too high!
    #define PRIORITY_DIAGNOSTICS        (configMAX_PRIORITIES - 2)  // 8 - Too high!
    #define PRIORITY_WATCHDOG           (configMAX_PRIORITIES - 1)  // 9 - Wrong!
    #define PRIORITY_HEALTH_MONITOR     (configMAX_PRIORITIES - 2)  // 8 - Too high!
    #define PRIORITY_ERROR_HANDLER      (configMAX_PRIORITIES - 1)  // 9 - Wrong!

    // Q3: High Urgency + Low Criticality - Way too high!
    #define PRIORITY_UI_EVENTS          (configMAX_PRIORITIES - 2)  // 8 - Too high!
    #define PRIORITY_DISPLAY_REFRESH    (configMAX_PRIORITIES - 1)  // 9 - Way too high!
    #define PRIORITY_BUTTON_HANDLER     (configMAX_PRIORITIES - 2)  // 8 - Too high!
    #define PRIORITY_ANIMATION          (configMAX_PRIORITIES - 1)  // 9 - Way too high!

    // Q4: Low Urgency + Low Criticality - Should be lowest!
    #define PRIORITY_DATA_LOGGING       (configMAX_PRIORITIES - 3)  // 7 - Should be low
    #define PRIORITY_FILE_CLEANUP       (configMAX_PRIORITIES - 2)  // 8 - Should be low
    #define PRIORITY_STATISTICS         (configMAX_PRIORITIES - 3)  // 7 - Should be low
    #define PRIORITY_TELEMETRY          (configMAX_PRIORITIES - 2)  // 8 - Should be low
#endif

/* ============================================================================
 * TASK EXECUTION TIME SIMULATION (in ms)
 * ============================================================================ */
#if USE_CPU_STRESS
    // HEAVILY STRESSED execution times to clearly demonstrate priority issues
    // Q1 tasks: Short, frequent, critical - Keep tight deadlines
    #define EXEC_TIME_SAFETY_CRITICAL   3       // Increased from 2
    #define EXEC_TIME_CAN_HANDLER       5       // Increased from 3
    #define EXEC_TIME_MOTOR_CONTROL     6       // Increased from 4
    #define EXEC_TIME_SENSOR_FUSION     5       // Increased from 3

    // Q2 tasks: Longer, less frequent, critical - Significantly increased
    #define EXEC_TIME_DIAGNOSTICS       150     // Increased from 80
    #define EXEC_TIME_WATCHDOG          8       // Increased from 5
    #define EXEC_TIME_HEALTH_MONITOR    120     // Increased from 60
    #define EXEC_TIME_ERROR_HANDLER     80      // Increased from 40

    // Q3 tasks: Medium length, frequent, non-critical - Moderately increased
    #define EXEC_TIME_UI_EVENTS         25      // Increased from 15
    #define EXEC_TIME_DISPLAY_REFRESH   30      // Increased from 20
    #define EXEC_TIME_BUTTON_HANDLER    20      // Increased from 12
    #define EXEC_TIME_ANIMATION         28      // Increased from 18

    // Q4 tasks: Long, infrequent, non-critical - Heavily increased
    #define EXEC_TIME_DATA_LOGGING      400     // Increased from 250
    #define EXEC_TIME_FILE_CLEANUP      600     // Increased from 400
    #define EXEC_TIME_STATISTICS        350     // Increased from 200
    #define EXEC_TIME_TELEMETRY         500     // Increased from 300

    // CPU-intensive calculation iterations - SIGNIFICANTLY INCREASED
    #define CPU_BURN_ITERATIONS         150000  // Increased from 50000
#else
    // Original light execution times
    // Q1 tasks
    #define EXEC_TIME_SAFETY_CRITICAL   2
    #define EXEC_TIME_CAN_HANDLER       3
    #define EXEC_TIME_MOTOR_CONTROL     4
    #define EXEC_TIME_SENSOR_FUSION     3

    // Q2 tasks
    #define EXEC_TIME_DIAGNOSTICS       50
    #define EXEC_TIME_WATCHDOG          5
    #define EXEC_TIME_HEALTH_MONITOR    40
    #define EXEC_TIME_ERROR_HANDLER     30

    // Q3 tasks
    #define EXEC_TIME_UI_EVENTS         10
    #define EXEC_TIME_DISPLAY_REFRESH   15
    #define EXEC_TIME_BUTTON_HANDLER    8
    #define EXEC_TIME_ANIMATION         12

    // Q4 tasks
    #define EXEC_TIME_DATA_LOGGING      100
    #define EXEC_TIME_FILE_CLEANUP      200
    #define EXEC_TIME_STATISTICS        150
    #define EXEC_TIME_TELEMETRY         180

    #define CPU_BURN_ITERATIONS         10000
#endif

/* ============================================================================
 * GLOBAL VARIABLES
 * ============================================================================ */
typedef struct {
    char taskName[32];
    uint32_t executionCount;
    uint32_t missedDeadlines;
    uint32_t totalExecutionTime;
    uint32_t maxExecutionTime;
    TickType_t lastExecutionTime;
} TaskStats_t;

TaskStats_t taskStats[16];  // Increased from 8 to accommodate new tasks
SemaphoreHandle_t statsMutex;
volatile int keepRunning = 1;

/* ============================================================================
 * SIGNAL HANDLER
 * ============================================================================ */
void sigintHandler(int sig_num) {
    (void)sig_num;
    printf("\n\n[SHUTDOWN] Received Ctrl+C. Shutting down gracefully...\n");
    keepRunning = 0;
}

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */

// CPU-intensive work to simulate real processing
void burnCPU(uint32_t iterations) {
    volatile double result = 0.0;
    for (uint32_t i = 0; i < iterations; i++) {
        result += sqrt((double)i) * 0.001;
    }
}

void recordTaskExecution(const char* taskName, uint32_t execTime) {
    xSemaphoreTake(statsMutex, portMAX_DELAY);
    for (int i = 0; i < 16; i++) {
        if (strcmp(taskStats[i].taskName, taskName) == 0) {
            taskStats[i].executionCount++;
            taskStats[i].totalExecutionTime += execTime;
            taskStats[i].lastExecutionTime = xTaskGetTickCount();
            if (execTime > taskStats[i].maxExecutionTime) {
                taskStats[i].maxExecutionTime = execTime;
            }
            break;
        }
    }
    xSemaphoreGive(statsMutex);
}

void recordMissedDeadline(const char* taskName) {
    xSemaphoreTake(statsMutex, portMAX_DELAY);
    for (int i = 0; i < 16; i++) {
        if (strcmp(taskStats[i].taskName, taskName) == 0) {
            taskStats[i].missedDeadlines++;
            break;
        }
    }
    xSemaphoreGive(statsMutex);
}

void initTaskStats(int index, const char* taskName) {
    strncpy(taskStats[index].taskName, taskName, 31);
    taskStats[index].executionCount = 0;
    taskStats[index].missedDeadlines = 0;
    taskStats[index].totalExecutionTime = 0;
    taskStats[index].maxExecutionTime = 0;
    taskStats[index].lastExecutionTime = 0;
}

void simulateWork(uint32_t durationMs, uint32_t cpuIntensity) {
    if (USE_CPU_STRESS) {
        // Mix of CPU burn and delay
        burnCPU(cpuIntensity);
        vTaskDelay(pdMS_TO_TICKS(durationMs / 2));
    } else {
        vTaskDelay(pdMS_TO_TICKS(durationMs));
    }
}

/* ============================================================================
 * QUADRANT 1 TASKS: High Urgency + High Criticality
 * ============================================================================ */
void Task_SafetyCritical(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q1_SafetyCritical";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(10);

    printf("[INIT] %s started - Priority: %lu (HIGHEST)\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_SAFETY_CRITICAL, CPU_BURN_ITERATIONS / 10);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(5)) {
            recordMissedDeadline(taskName);
            printf("[CRITICAL] %s MISSED DEADLINE! Exec: %lu ms (limit: 5ms)\n",
                   taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_CANHandler(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q1_CAN_Handler";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(20);

    printf("[INIT] %s started - Priority: %lu\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_CAN_HANDLER, CPU_BURN_ITERATIONS / 8);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(10)) {
            recordMissedDeadline(taskName);
            printf("[CRITICAL] %s MISSED DEADLINE! Exec: %lu ms (limit: 10ms)\n",
                   taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_MotorControl(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q1_MotorControl";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(15);

    printf("[INIT] %s started - Priority: %lu\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_MOTOR_CONTROL, CPU_BURN_ITERATIONS / 6);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(8)) {
            recordMissedDeadline(taskName);
            printf("[CRITICAL] %s MISSED DEADLINE! Exec: %lu ms (limit: 8ms)\n",
                   taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_SensorFusion(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q1_SensorFusion";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(12);

    printf("[INIT] %s started - Priority: %lu (HIGHEST)\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_SENSOR_FUSION, CPU_BURN_ITERATIONS / 8);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(6)) {
            recordMissedDeadline(taskName);
            printf("[CRITICAL] %s MISSED DEADLINE! Exec: %lu ms (limit: 6ms)\n",
                   taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

/* ============================================================================
 * QUADRANT 2 TASKS: Low Urgency + High Criticality
 * ============================================================================ */
void Task_Diagnostics(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q2_Diagnostics";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(500);

    printf("[INIT] %s started - Priority: %lu\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_DIAGNOSTICS, CPU_BURN_ITERATIONS * 2);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(200)) {
            recordMissedDeadline(taskName);
            printf("[WARNING] %s MISSED DEADLINE! Exec: %lu ms (limit: 200ms)\n",
                   taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_Watchdog(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q2_Watchdog";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(100);

    printf("[INIT] %s started - Priority: %lu\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_WATCHDOG, CPU_BURN_ITERATIONS / 5);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(50)) {
            recordMissedDeadline(taskName);
            printf("[WARNING] %s MISSED DEADLINE! Exec: %lu ms (limit: 50ms)\n",
                   taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_HealthMonitor(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q2_HealthMonitor";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(750);

    printf("[INIT] %s started - Priority: %lu\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_HEALTH_MONITOR, CPU_BURN_ITERATIONS * 2);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(300)) {
            recordMissedDeadline(taskName);
            printf("[WARNING] %s MISSED DEADLINE! Exec: %lu ms (limit: 300ms)\n",
                   taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_ErrorHandler(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q2_ErrorHandler";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(250);

    printf("[INIT] %s started - Priority: %lu\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_ERROR_HANDLER, CPU_BURN_ITERATIONS);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(100)) {
            recordMissedDeadline(taskName);
            printf("[WARNING] %s MISSED DEADLINE! Exec: %lu ms (limit: 100ms)\n",
                   taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

/* ============================================================================
 * QUADRANT 3 TASKS: High Urgency + Low Criticality
 * ============================================================================ */
void Task_UIEvents(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q3_UI_Events";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(50);

    printf("[INIT] %s started - Priority: %lu\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_UI_EVENTS, CPU_BURN_ITERATIONS);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(100)) {
            recordMissedDeadline(taskName);
            printf("[INFO] %s sluggish response! Exec: %lu ms\n", taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_DisplayRefresh(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q3_Display";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(33);

    printf("[INIT] %s started - Priority: %lu\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_DISPLAY_REFRESH, CPU_BURN_ITERATIONS);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(33)) {
            recordMissedDeadline(taskName);
            printf("[INFO] %s DROPPED FRAME! Exec: %lu ms\n", taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_ButtonHandler(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q3_ButtonHandler";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(40);

    printf("[INIT] %s started - Priority: %lu\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_BUTTON_HANDLER, CPU_BURN_ITERATIONS / 2);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(80)) {
            recordMissedDeadline(taskName);
            printf("[INFO] %s sluggish response! Exec: %lu ms\n", taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_Animation(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q3_Animation";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(25);

    printf("[INIT] %s started - Priority: %lu\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_ANIMATION, CPU_BURN_ITERATIONS);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        if (executionTime > pdMS_TO_TICKS(25)) {
            recordMissedDeadline(taskName);
            printf("[INFO] %s frame skip! Exec: %lu ms\n", taskName, execMs);
        }

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

/* ============================================================================
 * QUADRANT 4 TASKS: Low Urgency + Low Criticality
 * ============================================================================ */
void Task_DataLogging(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q4_DataLogging";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(1000);

    printf("[INIT] %s started - Priority: %lu (LOWEST - Background)\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_DATA_LOGGING, CPU_BURN_ITERATIONS * 5);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_FileCleanup(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q4_FileCleanup";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(5000);

    printf("[INIT] %s started - Priority: %lu (LOWEST - Background)\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_FILE_CLEANUP, CPU_BURN_ITERATIONS * 10);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_Statistics(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q4_Statistics";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(2000);

    printf("[INIT] %s started - Priority: %lu (LOWEST - Background)\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_STATISTICS, CPU_BURN_ITERATIONS * 5);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

void Task_Telemetry(void *pvParameters) {
    (void)pvParameters;
    const char* taskName = "Q4_Telemetry";
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(3000);

    printf("[INIT] %s started - Priority: %lu (LOWEST - Background)\n", taskName, uxTaskPriorityGet(NULL));

    while (keepRunning) {
        TickType_t startTime = xTaskGetTickCount();
        simulateWork(EXEC_TIME_TELEMETRY, CPU_BURN_ITERATIONS * 8);

        TickType_t executionTime = xTaskGetTickCount() - startTime;
        uint32_t execMs = executionTime * portTICK_PERIOD_MS;

        recordTaskExecution(taskName, execMs);
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

/* ============================================================================
 * MONITORING TASK
 * ============================================================================ */
void Task_Monitor(void *pvParameters) {
    (void)pvParameters;
    const TickType_t xPeriod = pdMS_TO_TICKS(5000);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    printf("\n========================================\n");
    printf("MONITORING TASK STARTED\n");
    printf("Configuration: %s\n",
           USE_PROPER_PRIORITIES ? "PROPER PRIORITIES ✓" : "IMPROPER PRIORITIES ✗");
    printf("CPU Stress: %s\n", USE_CPU_STRESS ? "ENABLED" : "DISABLED");
    printf("========================================\n\n");

    vTaskDelay(pdMS_TO_TICKS(5000));

    while (keepRunning) {
        printf("\n\n");
        printf("========================================\n");
        printf("SYSTEM STATISTICS (Time: %lu ms)\n", xTaskGetTickCount() * portTICK_PERIOD_MS);
        printf("========================================\n");
        printf("%-25s %8s %8s %10s %8s\n", "Task Name", "Exec", "Missed", "Total(ms)", "Max(ms)");
        printf("----------------------------------------\n");

        xSemaphoreTake(statsMutex, portMAX_DELAY);

        uint32_t totalExec = 0;
        uint32_t totalMissed = 0;

        for (int i = 0; i < 16; i++) {
            if (taskStats[i].taskName[0] != '\0') {
                printf("%-25s %8lu %8lu %10lu %8lu\n",
                       taskStats[i].taskName,
                       taskStats[i].executionCount,
                       taskStats[i].missedDeadlines,
                       taskStats[i].totalExecutionTime,
                       taskStats[i].maxExecutionTime);
                totalExec += taskStats[i].executionCount;
                totalMissed += taskStats[i].missedDeadlines;
            }
        }

        xSemaphoreGive(statsMutex);

        printf("----------------------------------------\n");
        printf("Total Executions: %lu\n", totalExec);
        printf("Total Missed Deadlines: %lu %s\n", totalMissed, totalMissed == 0 ? "✓" : "✗");
        printf("========================================\n");

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
    vTaskDelete(NULL);
}

/* ============================================================================
 * IDLE HOOK
 * ============================================================================ */
void vApplicationIdleHook(void) {
    // Idle task runs when no other task is ready
    usleep(1000);
}

/* ============================================================================
 * STACK OVERFLOW HOOK
 * ============================================================================ */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("[ERROR] Stack overflow in task: %s\n", pcTaskName);
    exit(1);
}

/* ============================================================================
 * MALLOC FAILED HOOK
 * ============================================================================ */
void vApplicationMallocFailedHook(void) {
    printf("[ERROR] Malloc failed!\n");
    exit(1);
}

/* ============================================================================
 * ASSERT
 * ============================================================================ */
void vAssertCalled(const char * const pcFileName, unsigned long ulLine) {
    printf("[ASSERT] %s:%lu\n", pcFileName, ulLine);
    exit(1);
}

/* ============================================================================
 * MAIN
 * ============================================================================ */
int main(void) {
    signal(SIGINT, sigintHandler);

    printf("\n");
    printf("====================================================================\n");
    printf("FreeRTOS Priority-Urgency Quadrant Model Demonstration - ENHANCED\n");
    printf("====================================================================\n");
    printf("Paper: 'The Hidden Challenge of RTOS: CPU Utilization'\n");
    printf("Author: Azad Mohammed Shaik\n");
    printf("====================================================================\n\n");

    // Initialize statistics
    statsMutex = xSemaphoreCreateMutex();
    if (statsMutex == NULL) {
        printf("[ERROR] Failed to create mutex\n");
        return 1;
    }

    // Q1 tasks (High Urgency + High Criticality)
    initTaskStats(0, "Q1_SafetyCritical");
    initTaskStats(1, "Q1_CAN_Handler");
    initTaskStats(2, "Q1_MotorControl");
    initTaskStats(3, "Q1_SensorFusion");

    // Q2 tasks (Low Urgency + High Criticality)
    initTaskStats(4, "Q2_Diagnostics");
    initTaskStats(5, "Q2_Watchdog");
    initTaskStats(6, "Q2_HealthMonitor");
    initTaskStats(7, "Q2_ErrorHandler");

    // Q3 tasks (High Urgency + Low Criticality)
    initTaskStats(8, "Q3_UI_Events");
    initTaskStats(9, "Q3_Display");
    initTaskStats(10, "Q3_ButtonHandler");
    initTaskStats(11, "Q3_Animation");

    // Q4 tasks (Low Urgency + Low Criticality)
    initTaskStats(12, "Q4_DataLogging");
    initTaskStats(13, "Q4_FileCleanup");
    initTaskStats(14, "Q4_Statistics");
    initTaskStats(15, "Q4_Telemetry");

    printf("Creating tasks with %s priorities...\n\n",
           USE_PROPER_PRIORITIES ? "PROPER" : "IMPROPER");

    // Create Q1 tasks (High Urgency + High Criticality)
    xTaskCreate(Task_SafetyCritical, "Q1_SafetyCritical",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_SAFETY_CRITICAL, NULL);
    xTaskCreate(Task_CANHandler, "Q1_CAN_Handler",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_CAN_HANDLER, NULL);
    xTaskCreate(Task_MotorControl, "Q1_MotorControl",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_MOTOR_CONTROL, NULL);
    xTaskCreate(Task_SensorFusion, "Q1_SensorFusion",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_SENSOR_FUSION, NULL);

    // Create Q2 tasks (Low Urgency + High Criticality)
    xTaskCreate(Task_Diagnostics, "Q2_Diagnostics",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_DIAGNOSTICS, NULL);
    xTaskCreate(Task_Watchdog, "Q2_Watchdog",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_WATCHDOG, NULL);
    xTaskCreate(Task_HealthMonitor, "Q2_HealthMonitor",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_HEALTH_MONITOR, NULL);
    xTaskCreate(Task_ErrorHandler, "Q2_ErrorHandler",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_ERROR_HANDLER, NULL);

    // Create Q3 tasks (High Urgency + Low Criticality)
    xTaskCreate(Task_UIEvents, "Q3_UI_Events",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_UI_EVENTS, NULL);
    xTaskCreate(Task_DisplayRefresh, "Q3_Display",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_DISPLAY_REFRESH, NULL);
    xTaskCreate(Task_ButtonHandler, "Q3_ButtonHandler",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_BUTTON_HANDLER, NULL);
    xTaskCreate(Task_Animation, "Q3_Animation",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_ANIMATION, NULL);

    // Create Q4 tasks (Low Urgency + Low Criticality)
    xTaskCreate(Task_DataLogging, "Q4_DataLogging",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_DATA_LOGGING, NULL);
    xTaskCreate(Task_FileCleanup, "Q4_FileCleanup",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_FILE_CLEANUP, NULL);
    xTaskCreate(Task_Statistics, "Q4_Statistics",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_STATISTICS, NULL);
    xTaskCreate(Task_Telemetry, "Q4_Telemetry",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_TELEMETRY, NULL);

    // Create monitoring task
    xTaskCreate(Task_Monitor, "Monitor",
                configMINIMAL_STACK_SIZE * 2, NULL, PRIORITY_WATCHDOG, NULL);

    printf("Starting FreeRTOS Scheduler...\n");
    printf("Press Ctrl+C to stop\n\n");

    vTaskStartScheduler();

    // Should never reach here
    printf("[ERROR] Scheduler failed to start!\n");
    return 1;
}
