#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* nondeterministic initial output value */
    size_t result = nondet_size_t();

    /* save old state */
    size_t old_n = n;
    size_t old_result = result;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, &result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        assert(result >= n);
        assert(aws_is_power_of_two(result));
    } else {
        assert(result == old_result);
    }

    /* inputs must remain unchanged */
    assert(n == old_n);
}
