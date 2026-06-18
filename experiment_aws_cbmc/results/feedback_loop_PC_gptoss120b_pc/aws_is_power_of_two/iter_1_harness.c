#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer and give it a nondeterministic initial value */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    *result = nondet_size_t();

    /* snapshot old state */
    size_t old_n = n;
    size_t old_result = *result;

    /* call function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(*result != 0);
        assert((*result & (*result - 1)) == 0);

        /* result must be >= n */
        assert(*result >= n);

        if (n == 0) {
            /* special case: n == 0 yields result == 1 */
            assert(*result == 1);
        } else {
            /* result is the smallest power of two >= n */
            assert((*result >> 1) < n);
        }
    } else {
        /* on failure, result must be unchanged */
        assert(*result == old_result);
    }

    /* input n must be unchanged */
    assert(n == old_n);

    free(result);
}
