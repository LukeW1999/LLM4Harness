#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate result storage and give it a nondeterministic initial value */
    size_t result;
    result = nondet_size_t();

    /* save old state */
    size_t old_n = n;
    size_t old_result = result;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, &result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two and >= n */
        assert(aws_is_power_of_two(result));
        assert(result >= old_n);
    } else {
        /* on error, result must be unchanged */
        assert(result == old_result);
    }

    /* n is an input parameter and must remain unchanged */
    assert(n == old_n);
}
