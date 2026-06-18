#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_BUFFER_SIZE);               /* bound to avoid UNKNOWN */

    size_t result_val = nondet_size_t();
    size_t *result = &result_val;
    size_t old_result = result_val;                       /* save old value */

    /* 2. Call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* 3. Post‑condition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* Success path */
        if (n == 0) {
            /* n == 0 ⇒ result must be 1 */
            assert(*result == 1);
        } else {
            /* n > 0 and n ≤ SIZE_MAX_POWER_OF_TWO */
            assert(*result >= n);                                 /* result ≥ n */
            assert((*result & (*result - 1)) == 0);                /* result is power of two */
            assert((*result >> 1) < n);                           /* smallest such power of two */
        }
    } else {
        /* Failure path */
        assert(rc == AWS_OP_ERR);
        /* No write to *result on overflow */
        assert(*result == old_result);
        /* Overflow occurs only when n > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }

    /* 4. Unchanged inputs */
    assert(n == n);                     /* n is unchanged (trivial sanity) */
    assert(result == &result_val);      /* pointer unchanged */
}
