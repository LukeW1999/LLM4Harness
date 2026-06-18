#include <aws/common/math.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    /* nondeterministically initialize the pointed-to value */
    *result = nondet_size_t();

    /* save old state */
    size_t old_n = n;
    size_t old_result = *result;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(*result));
        /* result must be >= n */
        assert(*result >= old_n);
        /* result must be the smallest power of two >= n */
        if (*result > 1) {
            assert(((*result) >> 1) < old_n);
        }
        /* n must not have changed */
        assert(n == old_n);
    } else {
        /* on failure, result must be unchanged */
        assert(*result == old_result);
        /* n must not have changed */
        assert(n == old_n);
    }

    /* clean up */
    free(result);
}
