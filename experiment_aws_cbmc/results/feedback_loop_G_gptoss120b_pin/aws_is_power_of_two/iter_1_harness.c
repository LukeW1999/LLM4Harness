#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer */
    size_t *result = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* nondeterministically initialize *result */
    *result = nondet_size_t();

    /* save old state */
    size_t old_n = n;
    size_t old_result_val = *result;
    size_t *old_result_ptr = result;

    /* call function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(*result));

        /* result must be >= original n */
        assert(*result >= old_n);

        if (old_n == 0) {
            /* special case: n == 0 yields result == 1 */
            assert(*result == 1);
        } else {
            /* result is the smallest power of two >= n */
            assert((*result >> 1) < old_n);
        }
    } else {
        /* on failure, *result must be unchanged */
        assert(*result == old_result_val);
    }

    /* unchanged pointers and inputs */
    assert(result == old_result_ptr);
    assert(n == old_n);

    /* clean up */
    free(result);
}
