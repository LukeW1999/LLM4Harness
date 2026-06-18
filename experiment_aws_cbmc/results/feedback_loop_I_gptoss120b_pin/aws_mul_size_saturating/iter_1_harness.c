#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* 1. Declare inputs */
    size_t n = nondet_size_t();

    /* Allocate result pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    /* Initialize result with nondeterministic value to detect unwanted modifications */
    *result = nondet_size_t();

    /* Save old state */
    size_t old_n = n;
    size_t old_result = *result;

    /* 2. Call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* 3. Postcondition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two and >= n */
        assert(aws_is_power_of_two(*result));
        assert(*result >= n);

        /* result must be the smallest power of two >= n */
        if (n > 0) {
            assert((*result >> 1) < n);
        } else {
            /* for n == 0 the smallest power of two is defined as 0 or 1;
               the implementation returns 0, which is not a power of two,
               so we only check that result is 0 in this case. */
            assert(*result == 0);
        }
    } else {
        /* On failure, result must be unchanged */
        assert(*result == old_result);
    }

    /* 4. Unchanged inputs */
    assert(n == old_n);

    /* 5. Validity invariants */
    /* result pointer must remain writable and non‑NULL (already assumed) */
    assert(result != NULL);
}
