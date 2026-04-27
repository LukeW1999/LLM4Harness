#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness() {
    size_t x = nondet_size_t();
    size_t result;

    /* Call function under test */
    int ret = aws_round_up_to_power_of_two(x, &result);

    /* Assert postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* Check that result is a power of two */
        assert((result & (result - 1)) == 0);
        /* Check that result is the smallest power of two greater than or equal to x */
        assert(result >= x);
        /* Check that there is no smaller power of two that is >= x */
        assert((result / 2) < x);
    } else {
        /* In case of failure, result should be unchanged */
        assert(result == 0);
    }
}
