# LatticeMCU Architecture

## Scope

LatticeMCU is a bare-metal framework for resource-constrained MCUs. The
portable core must build without vendor headers, vendor startup code, or a
specific board definition.

The default execution model is a cooperative event loop with static memory.
An RTOS adapter may be added later, but application and service interfaces
should not depend on a particular scheduler implementation.

## Layers

```text
application
    services
        drivers
            hal
                bsp
                    port
                        mcu
```

### `core`

Defines fixed-width types, status codes, compiler attributes, assertions, and
critical-section contracts. It has no hardware dependency.

### `kernel`

Provides the scheduler, software timers, event queues, deferred work, and the
idle hook. Kernel modules consume abstract tick and critical-section services
from `port`.

### `hal`

Defines hardware interfaces such as GPIO, UART, ADC, timer, flash, PWM, and
watchdog. Headers describe behavior and ownership; implementations are target
specific.

### `drivers`

Implements device behavior using HAL interfaces. Examples include an encoder,
motor controller, ADC-key decoder, and power controller. Drivers do not call
vendor register APIs directly.

### `services`

Implements reusable product services such as logging, binary protocols,
parameter storage, and firmware update. Services communicate with drivers and
the kernel through public interfaces.

### `bsp` and `port`

`port` contains CPU/compiler primitives: startup handoff, interrupt entry,
tick source, atomics, and critical sections. `bsp` selects concrete pins,
peripherals, clocks, and board policy for a target.

## Runtime rules

1. Interrupt handlers clear hardware state, capture minimal data, and enqueue
   an event or byte.
2. Tasks process bounded amounts of work and return without blocking.
3. UART TX and other slow peripherals use asynchronous buffers.
4. Dynamic allocation is disabled by default; modules receive caller-owned
   buffers during initialization.
5. All time comparisons use unsigned wrap-safe subtraction.
6. Each queue exposes overflow statistics so loss is observable.

## Initialization order

```text
platform/port
    -> kernel
    -> bsp
    -> hal-backed drivers
    -> services
    -> application
    -> scheduler start
```

Application code should only depend on services and drivers. A target port can
then be added without changing the portable kernel.

## Current implementation slice

The repository currently contains the static cooperative scheduler. Its tick
source is injected through `lmcu_tick_fn`, which keeps the scheduler usable in
host tests and on any MCU. Event queues, timers, HAL contracts, and target
ports will be added behind the same portability boundary.
