#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <aws/common/math.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_add_size_saturating_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Non‑deterministic expectation of overflow */
    bool expect_overflow = nondet_bool();

    /* 3. Constrain inputs so both branches are reachable */
    if (expect_overflow) {
        __CPROVER_assume(a > SIZE_MAX - b);
    } else {
        __CPROVER_assume(a <= SIZE_MAX - b);
    }

    /* 4. Call the function under test */
    size_t result = 0;
    bool overflow = aws_add_size_saturating(a, b, &result);

    /* 5. Post‑conditions */
    assert(overflow == expect_overflow);
    if (overflow) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a + b);
    }

    (void)result; /* suppress unused variable warning */
}
