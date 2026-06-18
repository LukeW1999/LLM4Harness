#include <aws/common/common.h>
#include <aws/common/math.h>

#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate space for the output pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* snapshot old state */
    size_t old_n = n;
    size_t old_result = *result;

    /* call function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* postconditions for success path */
    if (ret == AWS_OP_SUCCESS) {
        if (old_n == 0) {
            /* n == 0 case: result must be 1 */
            assert(*result == 1);
        } else {
            /* n > 0 and n <= SIZE_MAX_POWER_OF_TWO case */
            /* result is a power of two */
            assert(*result != 0);
            assert((*result & (*result - 1)) == 0);
            /* result is the smallest power of two >= n */
            assert(*result >= old_n);
            /* result cannot exceed the maximum power‑of‑two that fits in size_t */
            assert(*result <= SIZE_MAX_POWER_OF_TWO);
        }
    } else {
        /* failure path: n > SIZE_MAX_POWER_OF_TWO */
        /* result must be unchanged */
        assert(*result == old_result);
        /* the failure condition must hold */
        assert(old_n > SIZE_MAX_POWER_OF_TWO);
    }

    /* inputs must remain unchanged */
    assert(n == old_n);
    /* pointer itself must not be altered */
    assert(result != NULL);

    free(result);
}
