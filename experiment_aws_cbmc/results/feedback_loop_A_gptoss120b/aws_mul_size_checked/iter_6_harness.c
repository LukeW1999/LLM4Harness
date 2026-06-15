#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>

/* Function under test */
bool aws_mul_size_checked(size_t a, size_t b, size_t *result);

/* Nondeterministic helpers (provided by the proof environment) */
bool nondet_bool(void);
size_t nondet_size_t(void);

/* Bounding macro (optional) */
#ifndef MAX_VAL
#define MAX_VAL 1024
#endif

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a <= MAX_VAL);
    __CPROVER_assume(b <= MAX_VAL);

    size_t result = nondet_size_t();   /* initial value of the output parameter */

    bool no_overflow;

    /* Choose a reachable path (no‑overflow / overflow) under the bounds */
    if (nondet_bool()) {
        /* No overflow reachable */
        __CPROVER_assume(a == 0 || b == 0 || a <= SIZE_MAX / b);
    } else {
        /* Overflow reachable */
        __CPROVER_assume(a != 0 && b != 0 && a > SIZE_MAX / b);
    }

    no_overflow = aws_mul_size_checked(a, b, &result);

    if (no_overflow) {
        /* When the function reports no overflow, the result must be the exact product */
        assert(result == a * b);
    } else {
        /* When overflow is reported, we make no claim about the value stored in result */
    }

    /* The flag must be consistent with the arithmetic condition (true == no overflow) */
    if (a == 0 || b == 0) {
        assert(no_overflow);
    } else {
        assert(no_overflow == !(a > SIZE_MAX / b));
    }
}
