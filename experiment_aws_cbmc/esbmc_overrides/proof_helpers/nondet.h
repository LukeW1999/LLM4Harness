#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
/* ESBMC already declares nondet_bool internally; only declare the rest */
extern uint8_t   nondet_uint8_t(void);
extern uint16_t  nondet_uint16_t(void);
extern uint32_t  nondet_uint32_t(void);
extern uint64_t  nondet_uint64_t(void);
extern size_t    nondet_size_t(void);
extern int       nondet_int(void);
extern int32_t   nondet_int32_t(void);
extern int64_t   nondet_int64_t(void);
extern uintptr_t nondet_uintptr_t(void);
