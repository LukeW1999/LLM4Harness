#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <aws/common/math.h>
#include <aws/common/allocator.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_add_size_saturating_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Choose whether we expect overflow */
    bool expect_overflow = nondet_bool();

    /* 3. Constrain inputs to make both branches reachable */
    if (expect_overflow) {
        /* Force overflow: a + b would exceed SIZE_MAX */
        __CPROVER_assume(a > SIZE_MAX - b);
    } else {
        /* No overflow */
        __CPROVER_assume(a <= SIZE_MAX - b);
    }

    /* 4. Call the function under test */
    size_t result = 0;
    bool overflow = aws_add_size_saturating(a, b, &result);

    /* 5. Post‑conditions */
    /* 5a. The overflow flag must match the expectation */
    assert(overflow == expect_overflow);

    /* 5b. Result must be saturated to SIZE_MAX on overflow, otherwise exact sum */
    if (overflow) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a + b);
    }

    /* 6. The result pointer itself must not be altered (it points to a stack variable) */
    (void)result; /* suppress unused variable warning */
}
