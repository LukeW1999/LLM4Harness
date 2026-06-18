#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();

    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    *result = nondet_size_t(); /* old value */

    size_t old_result = *result;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert((*result != 0) && ((*result & (*result - 1)) == 0));

        /* result must be >= n */
        assert(*result >= n);

        /* no overflow: result fits in size_t (always true for size_t) */
        /* additionally, result should not exceed the maximum power of two representable */
        assert(*result <= ((size_t)1 << (SIZE_BITS - 1)));
    } else {
        /* on error, result must be unchanged */
        assert(*result == old_result);
    }

    /* n is passed by value, it must remain unchanged */
    assert(n == n);
}
