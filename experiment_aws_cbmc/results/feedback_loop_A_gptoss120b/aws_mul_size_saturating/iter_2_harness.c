#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include "aws/common/common.h"

/* Proof helper declarations (normally provided by the proof harness framework) */
bool nondet_bool(void);
size_t nondet_size_t(void);
void __CPROVER_assume(_Bool);
void assert(_Bool);

void aws_mul_size_saturating_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Choose whether we want the overflow or non‑overflow case */
    bool want_overflow = nondet_bool();

    if (want_overflow) {
        /* Force an overflow condition */
        __CPROVER_assume(a != 0);
        __CPROVER_assume(b > SIZE_MAX / a);
    } else {
        /* Force a non‑overflow condition */
        __CPROVER_assume(a == 0 || b <= SIZE_MAX / a);
    }

    /* 3. Allocate space for the result */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* 4. Remember original inputs */
    size_t old_a = a;
    size_t old_b = b;

    /* 5. Call the function under test */
    int rc = aws_mul_size_saturating(a, b, result);

    /* 6. Post‑condition checks */
    /* Inputs must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);

    if (a != 0 && b > SIZE_MAX / a) {
        /* Overflow case */
        assert(rc != 0);
        assert(*result == SIZE_MAX);
    } else {
        /* No overflow case */
        assert(rc == 0);
        assert(*result == a * b);
    }

    /* 7. Clean up */
    free(result);
}
