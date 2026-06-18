#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* 1. Declare and bound inputs */
    size_t n = nondet_size_t();
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    /* result must be writable */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(result, sizeof(size_t)));

    /* 2. Save old state of *result (before call) */
    size_t old_result = *result;

    /* 3. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (ret == AWS_OP_SUCCESS) {
        /* On success, *result is a power of two >= n */
        assert(aws_is_power_of_two(*result));
        assert(*result >= n);
        /* If n == 0, result must be 1 */
        if (n == 0) {
            assert(*result == 1);
        }
    } else {
        /* On failure, ret must be AWS_OP_ERR */
        assert(ret == AWS_OP_ERR);
        /* On failure, *result must be unchanged (no partial write) */
        assert(*result == old_result);
    }

    /* 5. Validity invariants: result pointer is still valid */
    assert(AWS_MEM_IS_WRITABLE(result, sizeof(size_t)));

    /* 6. Free allocated memory */
    free(result);
}
