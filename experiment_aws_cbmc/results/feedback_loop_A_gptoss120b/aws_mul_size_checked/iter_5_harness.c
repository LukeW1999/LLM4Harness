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

    bool overflow;

    /* Choose a path (overflow / no‑overflow) that is reachable under the bounds */
    if (nondet_bool()) {
        __CPROVER_assume(a == 0 || b == 0 || a <= SIZE_MAX / b);   /* no overflow */
    } else {
        __CPROVER_assume(a != 0 && b != 0 && a > SIZE_MAX / b);    /* overflow */
    }

    overflow = aws_mul_size_checked(a, b, &result);

    if (!overflow) {
        /* When no overflow is reported, the result must be the exact product */
        assert(result == a * b);
    } else {
        /* When overflow is reported, we make no claim about the value stored in result */
        /* (the implementation may leave it unchanged, set it to a truncated product,
           or any other value). */
    }

    /* The overflow flag must be consistent with the arithmetic condition */
    if (a == 0 || b == 0) {
        assert(!overflow);
    } else {
        assert(overflow == (a > SIZE_MAX / b));
    }
}
