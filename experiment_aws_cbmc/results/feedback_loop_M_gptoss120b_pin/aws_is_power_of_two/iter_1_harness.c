#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input, bounded to keep state space finite */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_BUFFER_SIZE);

    /* result buffer, also bounded */
    size_t result = nondet_size_t();
    __CPROVER_assume(result <= MAX_BUFFER_SIZE);
    size_t old_result = result;

    /* call the function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* post‑condition checks */
    if (ret == AWS_OP_SUCCESS) {
        if (n == 0) {
            assert(result == 1);
        } else {
            /* result must be a power of two and >= n */
            assert(result >= n);
            assert(aws_is_power_of_two(result));
            /* minimality: the previous power of two is < n */
            assert((result >> 1) < n);
        }
    } else {
        /* failure occurs only when n is too large to round up */
        assert(n > SIZE_MAX_POWER_OF_TWO);
        /* result must be unchanged on failure */
        assert(result == old_result);
    }

    /* n is an input and must remain unchanged */
    assert(n == n);
}
