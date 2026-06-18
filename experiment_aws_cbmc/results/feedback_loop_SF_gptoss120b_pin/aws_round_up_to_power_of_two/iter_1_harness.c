#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* Nondeterministic input */
    size_t n = (size_t) nondet_uint64_t();

    /* Allocate result pointer */
    size_t *result = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* Precondition assumptions (if any) can be added here */
    __CPROVER_assume(n <= SIZE_MAX);

    /* Snapshot of inputs */
    size_t n_old = n;
    size_t result_old = *result;

    /* Call the function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* input n must be unchanged */
    assert(n == n_old);

    /* result pointer must remain non‑NULL */
    assert(result != NULL);

    /* return value must be either success or an error code */
    assert(ret == AWS_OP_SUCCESS ||
           ret == AWS_ERROR_INVALID_ARGUMENT ||
           ret == AWS_ERROR_OVERFLOW ||
           ret == AWS_ERROR_SHORT_BUFFER ||
           ret == AWS_ERROR_MISSING_DATA ||
           ret == AWS_ERROR_UNIMPLEMENTED ||
           ret == AWS_ERROR_UNKNOWN);

    if (ret == AWS_OP_SUCCESS) {
        /* *result must be a power of two */
        assert((*result & (*result - 1)) == 0);
        /* and it must be at least the original n */
        assert(*result >= n_old);
        /* special case: when n == 0 the result should be 0 */
        if (n_old == 0) {
            assert(*result == 0);
        }
    } else {
        /* on error the result must be unchanged (frame condition) */
        assert(*result == result_old);
    }
}
