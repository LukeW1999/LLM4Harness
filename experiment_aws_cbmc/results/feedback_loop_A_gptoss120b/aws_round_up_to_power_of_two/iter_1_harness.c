#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();

    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* save old state of *result for the failure case */
    size_t old_result = *result;

    /* call the function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* post‑condition checks */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(*result != 0);
        assert((*result & (*result - 1)) == 0);

        /* result must be >= n */
        assert(*result >= n);

        /* special case when n == 0 */
        if (n == 0) {
            assert(*result == 1);
        } else {
            /* overflow could not have happened, therefore n is bounded */
            assert(n <= SIZE_MAX_POWER_OF_TWO);
        }
    } else {
        /* failure occurs only when n is too large to round up without overflow */
        assert(n > SIZE_MAX_POWER_OF_TWO);

        /* *result must remain unchanged on failure */
        assert(*result == old_result);
    }

    /* clean up */
    free(result);
}
