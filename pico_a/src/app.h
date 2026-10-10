#ifndef APP_H
#define APP_H
/* Definitions shared by the whole Pico A firmware. Change only by team agreement. */
#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"

/* Task priorities (FreeRTOS: larger number = higher priority). Design review, Section 3. */
#define PRIO_PLANNER  3
#define PRIO_LINK     4
#define PRIO_MOTOR    5
#define PRIO_SAFETY   6

/* Angles are centi-degrees (1/100 degree) in int16 everywhere on the wire and in queues. */
typedef struct { int16_t tilt_cdeg; int16_t sweep_cdeg; } pose_t;

/* Command from the link to the planner. */
typedef struct { uint8_t seq; pose_t target; } move_cmd_t;
/* Setpoint from the planner to the motor task. */
typedef struct { uint8_t seq; pose_t target; } setpoint_t;

/* Fault flags (event group bits), set by any task, acted on by the safety task. */
#define FAULT_LINK_TIMEOUT  (1u << 0)
#define FAULT_ENCODER_1     (1u << 1)
#define FAULT_ENCODER_2     (1u << 2)
#define FAULT_LIMIT         (1u << 3)
#define FAULT_DRIVER        (1u << 4)
#define FAULT_STOP_CMD      (1u << 5)

/* Shared objects, created in app_main.c. */
extern QueueHandle_t      q_move;        /* link -> planner, move_cmd_t   */
extern QueueHandle_t      q_setpoint;    /* planner -> motor, setpoint_t  */
extern EventGroupHandle_t eg_faults;     /* FAULT_* bits                  */
extern volatile pose_t    g_pose;        /* measured pose, written by the motor task only */
extern volatile bool      g_settled;     /* written by the motor task only */
extern volatile uint32_t  g_last_heartbeat_ms; /* written by the link task only */

/* Safety API (subsystem4): the only code allowed to drive the EN pins. */
void safety_disable_motors(void);
bool safety_motors_enabled(void);
void safety_request_enable(void);  /* honoured only if no fault is latched */

/* Task entry points. */
void task_link(void *arg);     /* subsystem1 */
void task_motor(void *arg);    /* subsystem2 */
void task_planner(void *arg);  /* subsystem2 */
void task_safety(void *arg);   /* subsystem4 */

#endif
