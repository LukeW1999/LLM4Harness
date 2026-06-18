#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();
    size_t old_n = n;

    /* result buffer, initialized nondeterministically to detect unchanged on failure */
    size_t result = nondet_size_t();
    size_t old_result = result;

    /* call the function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* n is passed by value, should remain unchanged */
    assert(n == old_n);

    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* result must be >= original n */
        assert(result >= old_n);

        /* special case when n == 0 */
        if (old_n == 0) {
            assert(result == 1);
        }
    } else {
        /* on failure, result must be unchanged */
        assert(result == old_result);
    }
}
