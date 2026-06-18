#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Non‑deterministic input bounded */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_BUFFER_SIZE);

    /* 2. Allocate output pointer and give it a bounded initial value */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    *result = nondet_size_t();
    __CPROVER_assume(*result <= MAX_BUFFER_SIZE);
    size_t old_result = *result;

    /* 3. Call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* 4. Post‑condition checks for both success and failure paths */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be the smallest power of two >= n */
        assert(*result >= n);
        assert(aws_is_power_of_two(*result));
        if (n == 0) {
            assert(*result == 1);
        } else {
            /* ensure no smaller power of two satisfies the condition */
            size_t half = *result >> 1;
            assert(half < n);
        }
    } else {
        /* on failure the output must be unchanged */
        assert(*result == old_result);
    }

    /* 5. Return code must be one of the defined outcomes */
    assert(rc == AWS_OP_SUCCESS || rc == AWS_OP_ERR);
}
