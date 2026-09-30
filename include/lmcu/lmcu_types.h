#ifndef LMCU_TYPES_H
#define LMCU_TYPES_H

#include <stdint.h>

enum lmcu_status {
    LMCU_STATUS_OK = 0,
    LMCU_STATUS_INVALID_ARGUMENT,
    LMCU_STATUS_CAPACITY,
    LMCU_STATUS_NOT_FOUND,
    LMCU_STATUS_EXISTS,
    LMCU_STATUS_BUSY,
};

typedef uint32_t (*lmcu_tick_fn)(void);

#endif
