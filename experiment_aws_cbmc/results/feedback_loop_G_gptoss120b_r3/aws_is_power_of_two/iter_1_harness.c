#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();

    size_t *result_ptr = malloc(sizeof(size_t));
    __CPROVER_assume(result_ptr != NULL);
    *result_ptr = nondet_size_t();

    /* save old state */
    size_t old_n = n;
    size_t old_result = *result_ptr;
    size_t *old_result_ptr = result_ptr; /* pointer itself must stay unchanged */

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result_ptr);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(*result_ptr != 0);
        assert((*result_ptr & (*result_ptr - 1)) == 0);

        /* result must be >= n (or 1 when n == 0) */
        if (old_n == 0) {
            assert(*result_ptr == 1);
        } else {
            assert(*result_ptr >= old_n);
        }

        /* result must not overflow size_t (cannot exceed the largest power of two) */
        assert(*result_ptr <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* on failure, result must be unchanged */
        assert(*result_ptr == old_result);
    }

    /* unchanged inputs */
    assert(n == old_n);
    assert(result_ptr == old_result_ptr);
}
