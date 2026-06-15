#include <stddef.h>
#include <assert.h>
#include <limits.h>
#include "aws/common/math.h"

/* Nondeterministic helpers (provided by CBMC environment) */
size_t nondet_size_t(void);
int   nondet_int(void);

void aws_mul_size_checked_harness(void) {
    /* 1. Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Optional bounds to keep the state space reasonable */
    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    /* 2. Prepare result storage */
    size_t result_storage = nondet_size_t();   /* initial value may be anything */
    size_t *result = &result_storage;

    /* Save old values for later comparison */
    size_t old_a      = a;
    size_t old_b      = b;
    size_t old_result = result_storage;

    /* 3. Call the function under test */
    int ret = aws_mul_size_checked(a, b, result);

    /* 4. Compute the expected overflow condition using safe arithmetic */
    int overflow = 0;
    if (a != 0 && b > SIZE_MAX / a) {
        overflow = 1;
    }

    /* 5. Post‑condition checks */
    assert(a == old_a);
    assert(b == old_b);

    if (ret) {
        /* No overflow reported */
        assert(!overflow);
        assert(*result == a * b);
    } else {
        /* Overflow reported */
        assert(overflow);
        /* The function must not modify the result when overflow occurs */
        assert(*result == old_result);
    }
}
