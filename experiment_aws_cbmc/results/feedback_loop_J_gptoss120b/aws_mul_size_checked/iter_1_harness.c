#include <aws/common/math.h>
#include <aws/common/byte_buf.h>
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
    size_t *result = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    /* initialize result with nondeterministic value */
    *result = nondet_size_t();

    /* save old state */
    size_t old_n = n;
    size_t old_result = *result;
    size_t *old_result_ptr = result;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(*result));
        /* result must be >= n */
        assert(*result >= n);
        /* result must be non‑zero */
        assert(*result != 0);
        /* minimality: if result > n then result/2 < n */
        if (*result > n) {
            assert((*result >> 1) < n);
        }
    } else {
        /* on failure, result must be unchanged */
        assert(*result == old_result);
    }

    /* unchanged inputs */
    assert(n == old_n);
    assert(result == old_result_ptr);

    /* validity of pointer */
    assert(result != NULL);
}
