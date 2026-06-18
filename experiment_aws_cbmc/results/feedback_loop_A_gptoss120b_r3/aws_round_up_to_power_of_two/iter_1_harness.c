#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();

    /* allocate a result location and give it a nondeterministic initial value */
    size_t result_storage = nondet_size_t();
    size_t *result = &result_storage;

    /* save old value for failure case comparison */
    size_t old_result = result_storage;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert((*result != 0) && ((*result & (*result - 1)) == 0));

        if (n == 0) {
            /* special case: n == 0 yields result == 1 */
            assert(*result == 1);
        } else {
            /* general case: result >= n */
            assert(*result >= n);
        }
    } else {
        /* on failure the result must be unchanged */
        assert(*result == old_result);
        /* failure occurs only when n is too large to round up without overflow */
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(rc == AWS_OP_ERR);
    }
}
