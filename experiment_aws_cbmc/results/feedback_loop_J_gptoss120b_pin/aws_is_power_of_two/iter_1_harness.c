#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer */
    size_t *result = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* save old state of *result */
    size_t old_result = *result;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(*result != 0);
        assert((*result & (*result - 1)) == 0);

        /* result must be >= original n (or 1 when n == 0) */
        if (n == 0) {
            assert(*result == 1);
        } else {
            assert(*result >= n);
        }

        /* result cannot exceed the largest power of two that fits in size_t */
        assert(*result <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* on failure, *result must be unchanged */
        assert(*result == old_result);
    }

    /* pointer itself must remain unchanged */
    assert(result != NULL);
}
