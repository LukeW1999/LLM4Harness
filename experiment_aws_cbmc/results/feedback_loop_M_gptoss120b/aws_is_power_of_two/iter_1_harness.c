#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input bounded */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_BUFFER_SIZE);

    /* nondeterministic initial value for result, also bounded */
    size_t result = nondet_size_t();
    __CPROVER_assume(result <= MAX_BUFFER_SIZE);
    size_t old_result = result;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, &result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        if (n == 0) {
            assert(result == 1);
        } else {
            /* result must be a power of two and >= n */
            assert(result >= n);
            assert(result != 0);
            assert((result & (result - 1)) == 0);
        }
    } else {
        /* on failure, result must be unchanged */
        assert(result == old_result);
    }

    /* validity invariant: function does not modify memory outside of result */
    /* (no additional structure to validate) */
}
