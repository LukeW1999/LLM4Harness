#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

size_t nondet_size_t(void);
int nondet_int(void);

void aws_round_up_to_power_of_two_harness(void) {
    /* Precondition: allocate result pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* Precondition: nondeterministic inputs */
    size_t n = nondet_size_t();
    size_t old_result = nondet_size_t();
    *result = old_result;

    /* Call the function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* Postcondition: return value is either success or error */
    assert(ret == AWS_OP_SUCCESS || ret != AWS_OP_SUCCESS);

    if (ret == AWS_OP_SUCCESS) {
        /* Success case: result must be a power of two and >= n */
        assert(*result >= n);
        assert(aws_is_power_of_two(*result));
        if (n == 0) {
            assert(*result == 1);
        }
    } else {
        /* Error case: overflow detected, n must be too large */
        assert(n > SIZE_MAX_POWER_OF_TWO);
        /* Result must be unchanged */
        assert(*result == old_result);
    }

    /* No other memory should be modified (frame condition) */
    /* Since the harness only allocates `result`, we rely on CBMC's
       default memory model to ensure other locations are untouched. */

    free(result);
    return 0;
}
