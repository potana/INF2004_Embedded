/* Pico WH A: entry point. Creates the shared objects and starts the four tasks.
 * Shared file: change only by team agreement (TEAM.md section 1). */
#include <stdio.h>
#include "pico/stdlib.h"
#include "app.h"
#include "task.h"

QueueHandle_t      q_move;
QueueHandle_t      q_setpoint;
EventGroupHandle_t eg_faults;
volatile pose_t    g_pose;
volatile bool      g_settled;
volatile uint32_t  g_last_heartbeat_ms;

void vApplicationMallocFailedHook(void) { for (;;) {} }
void vApplicationStackOverflowHook(TaskHandle_t t, char *name) { (void)t; (void)name; for (;;) {} }

int main(void)
{
    stdio_init_all();

    q_move     = xQueueCreate(4, sizeof(move_cmd_t));
    q_setpoint = xQueueCreate(1, sizeof(setpoint_t));   /* latest setpoint wins (xQueueOverwrite) */
    eg_faults  = xEventGroupCreate();

    xTaskCreate(task_safety,  "safety",  512, NULL, PRIO_SAFETY,  NULL);
    xTaskCreate(task_motor,   "motor",   512, NULL, PRIO_MOTOR,   NULL);
    xTaskCreate(task_link,    "link",    512, NULL, PRIO_LINK,    NULL);
    xTaskCreate(task_planner, "planner", 512, NULL, PRIO_PLANNER, NULL);

    vTaskStartScheduler();
    for (;;) {}
}
