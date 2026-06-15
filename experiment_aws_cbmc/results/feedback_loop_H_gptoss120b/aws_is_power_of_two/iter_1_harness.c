#include <assert.h>
#include <stddef.h>
#include "aws/common/common.h"
#include "aws/common/math.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_round_up_to_power_of_two_harness() {
    /* 1. Non‑deterministic inputs */
    size_t n = nondet_size_t();
    /* Bound the input to keep the state space finite */
    __CPROVER_assume(n <= SIZE_MAX);

    /* Allocate storage for the output pointer */
    size_t result_storage;
    size_t *result = &result_storage;

    /* Save old state */
    size_t old_n = n;
    size_t old_result = *result;

    /* 2. Call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* 3. Post‑conditions for both success and failure paths */
    if (rc == AWS_OP_SUCCESS) {
        /* The result must be a power of two */
        assert((*result != 0) && ((*result & (*result - 1)) == 0));

        /* The result must be greater than or equal to the original n */
        assert(*result >= old_n);

        /* Special case when n == 0 */
        if (old_n == 0) {
            assert(*result == 1);
        }
    } else {
        /* On failure the output value must be unchanged */
        assert(*result == old_result);
    }

    /* 4. Fields that must remain unchanged regardless of the outcome */
    assert(n == old_n);          /* input n is not modified */
    assert(result == &result_storage); /* pointer itself is unchanged */

    /* 5. Validity invariants (trivially true for scalar values) */
    /* No additional structure to validate */
}
