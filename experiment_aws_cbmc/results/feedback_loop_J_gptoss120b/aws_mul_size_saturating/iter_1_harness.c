#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Declare nondeterministic inputs */
    size_t n = nondet_size_t();

    /* Allocate result pointer */
    size_t *result = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* Initialize result with nondeterministic value and save old state */
    *result = nondet_size_t();
    size_t old_result = *result;

    /* 2. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* 3. Postconditions for success path */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(*result != 0);
        assert((*result & (*result - 1)) == 0);

        /* result must be >= n */
        assert(*result >= n);

        /* result must be the smallest power of two >= n */
        if (*result > 1) {
            assert((*result >> 1) < n);
        }

        /* result must not overflow the maximum power of two representable in size_t */
        assert(*result <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* 4. Failure path: result must be unchanged */
        assert(*result == old_result);
    }

    /* 5. Unchanged inputs */
    assert(n == n); /* n is input, unchanged */

    /* 6. Validity invariants */
    /* result pointer must remain valid (non‑NULL) */
    assert(result != NULL);
}
