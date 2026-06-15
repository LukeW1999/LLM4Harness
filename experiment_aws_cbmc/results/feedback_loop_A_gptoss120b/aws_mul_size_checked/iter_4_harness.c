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
    size_t old_result = result;        /* save to check unchanged on overflow */

    bool overflow;

    /* Ensure both overflow and non‑overflow paths are reachable */
    if (nondet_bool()) {
        __CPROVER_assume(a == 0 || b == 0 || a <= SIZE_MAX / b);   /* no overflow */
    } else {
        __CPROVER_assume(a != 0 && b != 0 && a > SIZE_MAX / b);    /* overflow */
    }

    overflow = aws_mul_size_checked(a, b, &result);

    if (!overflow) {
        /* No overflow: result must equal the exact product */
        assert(result == a * b);
    } else {
        /* Overflow: result must remain unchanged */
        assert(result == old_result);
    }

    /* The overflow flag must correctly reflect the arithmetic condition */
    if (a == 0 || b == 0) {
        assert(!overflow);
    } else {
        assert(overflow == (a > SIZE_MAX / b));
    }
}
