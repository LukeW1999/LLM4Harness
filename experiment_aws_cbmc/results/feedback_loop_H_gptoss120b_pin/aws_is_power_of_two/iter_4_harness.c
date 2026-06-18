#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();
    size_t result = nondet_size_t();

    /* save old state */
    size_t old_result = result;
    size_t old_n = n;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, &result);

    /* postconditions for success path */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* result must be at least 1 */
        assert(result >= 1);

        /* result must be >= original n (or 1 when n == 0) */
        if (old_n == 0) {
            assert(result == 1);
        } else {
            assert(result >= old_n);
        }
    } else {
        /* on failure, result must be unchanged */
        assert(result == old_result);
    }

    /* failure occurs only when n > AWS_MAX_POWER_OF_TWO and n != 0 */
    if (rc != AWS_OP_SUCCESS) {
        assert(old_n != 0);
        assert(old_n > AWS_MAX_POWER_OF_TWO);
    } else {
        /* success cannot happen when n > AWS_MAX_POWER_OF_TWO */
        assert(!(old_n > AWS_MAX_POWER_OF_TWO));
    }
}
