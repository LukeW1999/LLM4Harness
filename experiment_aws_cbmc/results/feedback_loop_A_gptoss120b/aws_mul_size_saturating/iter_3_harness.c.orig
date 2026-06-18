#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "aws/common/common.h"

/* Nondeterministic helpers (provided by CBMC) */
size_t nondet_size_t(void);
bool nondet_bool(void);
void __CPROVER_assume(_Bool);
void assert(_Bool);

void aws_mul_size_saturating_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Allocate space for the result */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* 3. Remember original inputs */
    size_t old_a = a;
    size_t old_b = b;

    /* 4. Call the function under test */
    int rc = aws_mul_size_saturating(a, b, result);

    /* 5. Post‑condition checks */
    assert(a == old_a);
    assert(b == old_b);

    bool overflow = (a != 0 && b > SIZE_MAX / a);
    if (overflow) {
        /* Overflow case */
        assert(rc != 0);
        assert(*result == SIZE_MAX);
    } else {
        /* No overflow case */
        assert(rc == 0);
        assert(*result == a * b);
    }

    /* 6. Clean up */
    free(result);
}
