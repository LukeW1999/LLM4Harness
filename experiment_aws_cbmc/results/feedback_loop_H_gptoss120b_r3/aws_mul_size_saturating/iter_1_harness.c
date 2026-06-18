#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Declare nondeterministic inputs */
    size_t n = nondet_size_t();

    /* Allocate output pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    /* Initialize *result with a nondeterministic value */
    *result = nondet_size_t();

    /* Save old state */
    size_t old_n = n;
    size_t old_result = *result;
    size_t *old_result_ptr = result;

    /* 2. Call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* 3. Postcondition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(*result != 0);
        assert((*result & (*result - 1)) == 0);
        /* result must be >= n */
        assert(*result >= n);
        /* No overflow occurred, so result fits in size_t (implicitly true) */
    } else {
        /* On failure, result must be unchanged */
        assert(*result == old_result);
    }

    /* 4. Unchanged inputs */
    assert(n == old_n);
    assert(result == old_result_ptr);

    /* 5. Clean up */
    free(result);
}
