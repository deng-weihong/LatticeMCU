# LatticeMCU

LatticeMCU is a small, portable framework for resource-constrained MCUs.
The public C API uses the `lmcu_` prefix.

The framework uses a bare-metal, event-driven runtime with static memory,
cooperative scheduling, wrap-safe deadlines, and hardware isolation through
`port`, `bsp`, and `hal` layers.

## Repository layout

```text
include/lmcu/   Public framework headers
src/core/       Common types and status helpers
src/kernel/     Scheduler, timers, queues, and deferred work
src/hal/        Hardware abstraction interfaces
src/ports/      CPU/MCU-specific implementations
src/bsp/        Board-specific configuration and startup
services/       Logging, protocols, storage, and OTA
drivers/        Product-independent device drivers
examples/       Small board/application examples
tests/          Host-side tests for portable modules
```

## Scheduler example

```c
static void heartbeat_task(void *context)
{
    (void)context;
}

static struct lmcu_task tasks[1];
static struct lmcu_scheduler scheduler;

void app_init(void)
{
    lmcu_scheduler_init(&scheduler, tasks, 1, platform_get_tick_ms);
    lmcu_scheduler_add(&scheduler, "heartbeat", heartbeat_task, 0, 500);
    lmcu_scheduler_start(&scheduler);
}

void app_run(void)
{
    lmcu_scheduler_run_once(&scheduler);
}
```

Tasks must be bounded and non-blocking. Interrupt handlers should capture
minimal data and defer processing to a task or work item.

## Status

The repository currently contains the first portable scheduler slice. MCU
specific code will be added under `ports/` and `bsp/` when a target is chosen.

## Languages

- [简体中文 README](README.md)
- [English README](README.en.md)
