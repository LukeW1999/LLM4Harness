#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    int rval = aws_round_up_to_power_of_two(n, &result);

    if (rval == AWS_OP_SUCCESS) {
        // Success path postconditions
        assert(result >= n);
        assert(aws_is_power_of_two(result));
        assert(result <= SIZE_MAX_POWER_OF_TWO || n == 0);
    } else {
        // Failure path postconditions
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == 0); // result is undefined on failure, but we set it to 0 for consistency
    }

    // Validity invariants
    // No additional structures to validate in this function
}
