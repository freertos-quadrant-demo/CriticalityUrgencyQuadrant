/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.
 * 
 * FreeRTOS Configuration for POSIX Port (Linux)
 * Priority-Urgency Quadrant Demo
 */

 #ifndef FREERTOS_CONFIG_H
 #define FREERTOS_CONFIG_H
 
 #include <stdint.h>
 
 /*-----------------------------------------------------------
  * Application specific definitions.
  *-----------------------------------------------------------*/
 
 /* Scheduler */
 #define configUSE_PREEMPTION                    1
 #define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
 #define configUSE_TICKLESS_IDLE                 0
 #define configCPU_CLOCK_HZ                      ( ( unsigned long ) 1000000000 )
 #define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 ) // 1ms tick
 #define configMAX_PRIORITIES                    ( 10 )
 #define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 4096 )
 #define configMAX_TASK_NAME_LEN                 ( 32 )
 #define configUSE_16_BIT_TICKS                  0
 #define configIDLE_SHOULD_YIELD                 1
 #define configUSE_TASK_NOTIFICATIONS            1
 #define configTASK_NOTIFICATION_ARRAY_ENTRIES   3
 #define configUSE_MUTEXES                       1
 #define configUSE_RECURSIVE_MUTEXES             1
 #define configUSE_COUNTING_SEMAPHORES           1
 #define configUSE_ALTERNATIVE_API               0
 #define configQUEUE_REGISTRY_SIZE               10
 #define configUSE_QUEUE_SETS                    0
 #define configUSE_TIME_SLICING                  1
 #define configUSE_NEWLIB_REENTRANT              0
 #define configENABLE_BACKWARD_COMPATIBILITY     0
 #define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5
 #define configSTACK_DEPTH_TYPE                  uint32_t
 #define configMESSAGE_BUFFER_LENGTH_TYPE        size_t
 
 /* Memory allocation */
 #define configSUPPORT_STATIC_ALLOCATION         0
 #define configSUPPORT_DYNAMIC_ALLOCATION        1
 #define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 2 * 1024 * 1024 ) )
 #define configAPPLICATION_ALLOCATED_HEAP        0
 
 /* Hook functions */
 #define configUSE_IDLE_HOOK                     1
 #define configUSE_TICK_HOOK                     0
 #define configCHECK_FOR_STACK_OVERFLOW          2
 #define configUSE_MALLOC_FAILED_HOOK            1
 #define configUSE_DAEMON_TASK_STARTUP_HOOK      0
 
 /* Run time and task stats gathering */
 #define configGENERATE_RUN_TIME_STATS           0
 #define configUSE_TRACE_FACILITY                1
 #define configUSE_STATS_FORMATTING_FUNCTIONS    1
 
 /* Co-routine definitions */
 #define configUSE_CO_ROUTINES                   0
 #define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )
 
 /* Software timer definitions */
 #define configUSE_TIMERS                        1
 #define configTIMER_TASK_PRIORITY               ( configMAX_PRIORITIES - 1 )
 #define configTIMER_QUEUE_LENGTH                10
 #define configTIMER_TASK_STACK_DEPTH            ( configMINIMAL_STACK_SIZE * 2 )
 
 /* Optional functions - include */
 #define INCLUDE_vTaskPrioritySet                1
 #define INCLUDE_uxTaskPriorityGet               1
 #define INCLUDE_vTaskDelete                     1
 #define INCLUDE_vTaskSuspend                    1
 #define INCLUDE_xResumeFromISR                  1
 #define INCLUDE_vTaskDelayUntil                 1
 #define INCLUDE_vTaskDelay                      1
 #define INCLUDE_xTaskGetSchedulerState          1
 #define INCLUDE_xTaskGetCurrentTaskHandle       1
 #define INCLUDE_uxTaskGetStackHighWaterMark     1
 #define INCLUDE_xTaskGetIdleTaskHandle          1
 #define INCLUDE_eTaskGetState                   1
 #define INCLUDE_xEventGroupSetBitFromISR        1
 #define INCLUDE_xTimerPendFunctionCall          1
 #define INCLUDE_xTaskAbortDelay                 1
 #define INCLUDE_xTaskGetHandle                  1
 #define INCLUDE_xTaskResumeFromISR              1
 
 /* Assert definition */
 extern void vAssertCalled(const char * const pcFileName, unsigned long ulLine);
 #define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )
 
 /* POSIX Port Specific */
 #define configUSE_POSIX_ERRNO                   1
 
 /* Prevent errno.h from being included */
 #define projCOVERAGE_TEST                       0
 
 #endif /* FREERTOS_CONFIG_H */