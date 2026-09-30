#ifndef LMCU_SCHEDULER_H
#define LMCU_SCHEDULER_H

#include <stdint.h>

#include "lmcu_types.h"

typedef void (*lmcu_task_handler)(void *context);

struct lmcu_task {
    const char *name;
    lmcu_task_handler handler;
    void *context;
    uint32_t period_ms;
    uint32_t next_deadline_ms;
    uint32_t run_count;
    uint32_t overrun_count;
    uint8_t registered;
    uint8_t enabled;
};

struct lmcu_scheduler {
    struct lmcu_task *tasks;
    uint8_t capacity;
    uint8_t count;
    lmcu_tick_fn get_tick_ms;
    uint8_t started;
};

enum lmcu_status lmcu_scheduler_init(
    struct lmcu_scheduler *scheduler,
    struct lmcu_task *tasks,
    uint8_t capacity,
    lmcu_tick_fn get_tick_ms
);

enum lmcu_status lmcu_scheduler_add(
    struct lmcu_scheduler *scheduler,
    const char *name,
    lmcu_task_handler handler,
    void *context,
    uint32_t period_ms
);

enum lmcu_status lmcu_scheduler_start(struct lmcu_scheduler *scheduler);
enum lmcu_status lmcu_scheduler_stop(struct lmcu_scheduler *scheduler);
enum lmcu_status lmcu_scheduler_set_enabled(
    struct lmcu_scheduler *scheduler,
    const char *name,
    uint8_t enabled
);

void lmcu_scheduler_run_once(struct lmcu_scheduler *scheduler);

#endif
