#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* nondeterministically initialize *result to any value */
    *result = nondet_size_t();

    /* save old state of *result */
    size_t old_result = *result;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(*result));

        if (n == 0) {
            /* special case: n == 0 yields result == 1 */
            assert(*result == 1);
        } else {
            /* result must be >= n and <= SIZE_MAX */
            assert(*result >= n);
            assert(*result <= SIZE_MAX);
        }
    } else {
        /* on failure, *result must be unchanged */
        assert(*result == old_result);
        /* failure only occurs when n > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }

    /* clean up */
    free(result);
}
