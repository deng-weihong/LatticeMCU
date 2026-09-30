#include "lmcu/lmcu_scheduler.h"

#include <stddef.h>
#include <string.h>

static uint8_t deadline_reached(uint32_t now, uint32_t deadline)
{
    return ((int32_t)(now - deadline) >= 0) ? 1U : 0U;
}

static struct lmcu_task *find_task(
    struct lmcu_scheduler *scheduler,
    const char *name
)
{
    uint8_t i;

    if((scheduler == NULL) || (name == NULL))
    {
        return NULL;
    }

    for(i = 0; i < scheduler->count; i++)
    {
        struct lmcu_task *task = &scheduler->tasks[i];

        if(task->registered && (strcmp(task->name, name) == 0))
        {
            return task;
        }
    }

    return NULL;
}

enum lmcu_status lmcu_scheduler_init(
    struct lmcu_scheduler *scheduler,
    struct lmcu_task *tasks,
    uint8_t capacity,
    lmcu_tick_fn get_tick_ms
)
{
    if((scheduler == NULL) || (tasks == NULL) || (capacity == 0) ||
       (get_tick_ms == NULL))
    {
        return LMCU_STATUS_INVALID_ARGUMENT;
    }

    scheduler->tasks = tasks;
    scheduler->capacity = capacity;
    scheduler->count = 0;
    scheduler->get_tick_ms = get_tick_ms;
    scheduler->started = 0;

    for(uint8_t i = 0; i < capacity; i++)
    {
        tasks[i].registered = 0;
        tasks[i].enabled = 0;
        tasks[i].run_count = 0;
        tasks[i].overrun_count = 0;
    }

    return LMCU_STATUS_OK;
}

enum lmcu_status lmcu_scheduler_add(
    struct lmcu_scheduler *scheduler,
    const char *name,
    lmcu_task_handler handler,
    void *context,
    uint32_t period_ms
)
{
    struct lmcu_task *task;

    if((scheduler == NULL) || (name == NULL) || (handler == NULL) ||
       (period_ms == 0))
    {
        return LMCU_STATUS_INVALID_ARGUMENT;
    }

    if(find_task(scheduler, name) != NULL)
    {
        return LMCU_STATUS_EXISTS;
    }

    if(scheduler->count >= scheduler->capacity)
    {
        return LMCU_STATUS_CAPACITY;
    }

    task = &scheduler->tasks[scheduler->count++];
    task->name = name;
    task->handler = handler;
    task->context = context;
    task->period_ms = period_ms;
    task->next_deadline_ms = 0;
    task->run_count = 0;
    task->overrun_count = 0;
    task->registered = 1;
    task->enabled = 0;

    return LMCU_STATUS_OK;
}

enum lmcu_status lmcu_scheduler_start(struct lmcu_scheduler *scheduler)
{
    uint32_t now;

    if((scheduler == NULL) || (scheduler->get_tick_ms == NULL))
    {
        return LMCU_STATUS_INVALID_ARGUMENT;
    }

    now = scheduler->get_tick_ms();
    for(uint8_t i = 0; i < scheduler->count; i++)
    {
        struct lmcu_task *task = &scheduler->tasks[i];

        task->next_deadline_ms = now + task->period_ms;
        task->enabled = 1;
    }
    scheduler->started = 1;

    return LMCU_STATUS_OK;
}

enum lmcu_status lmcu_scheduler_stop(struct lmcu_scheduler *scheduler)
{
    if(scheduler == NULL)
    {
        return LMCU_STATUS_INVALID_ARGUMENT;
    }

    for(uint8_t i = 0; i < scheduler->count; i++)
    {
        scheduler->tasks[i].enabled = 0;
    }
    scheduler->started = 0;

    return LMCU_STATUS_OK;
}

enum lmcu_status lmcu_scheduler_set_enabled(
    struct lmcu_scheduler *scheduler,
    const char *name,
    uint8_t enabled
)
{
    struct lmcu_task *task = find_task(scheduler, name);

    if(task == NULL)
    {
        return LMCU_STATUS_NOT_FOUND;
    }

    task->enabled = enabled ? 1U : 0U;
    if(task->enabled && scheduler->get_tick_ms != NULL)
    {
        task->next_deadline_ms = scheduler->get_tick_ms() + task->period_ms;
    }

    return LMCU_STATUS_OK;
}

void lmcu_scheduler_run_once(struct lmcu_scheduler *scheduler)
{
    uint32_t now;

    if((scheduler == NULL) || !scheduler->started ||
       (scheduler->get_tick_ms == NULL))
    {
        return;
    }

    now = scheduler->get_tick_ms();
    for(uint8_t i = 0; i < scheduler->count; i++)
    {
        struct lmcu_task *task = &scheduler->tasks[i];
        uint32_t after_run;

        if(!task->enabled || !deadline_reached(now, task->next_deadline_ms))
        {
            continue;
        }

        task->next_deadline_ms += task->period_ms;
        task->handler(task->context);
        task->run_count++;

        after_run = scheduler->get_tick_ms();
        if(deadline_reached(after_run, task->next_deadline_ms))
        {
            task->overrun_count++;
            task->next_deadline_ms = after_run + task->period_ms;
        }
    }
}
