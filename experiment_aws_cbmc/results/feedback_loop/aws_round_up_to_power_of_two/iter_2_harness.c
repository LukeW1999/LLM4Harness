#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    size_t test_val = nondet_size_t();
    size_t result;
    int rval = aws_round_up_to_power_of_two(test_val, &result);

#if ULONG_MAX == SIZE_MAX
    int popcount = __builtin_popcountl(result);
#elif ULLONG_MAX == SIZE_MAX
    int popcount = __builtin_popcountll(result);
#else
#    error
#endif

    if (rval == AWS_OP_SUCCESS) {
        assert(popcount == 1);          // result is a power of two
        assert(test_val <= result);      // result is not less than test_val
        assert(test_val >= result >> 1); // result is the smallest power of two greater than or equal to test_val
    } else {
        // Only fail if rounding up would cause us to overflow.
        assert(test_val > ((SIZE_MAX >> 1) + 1));
    }
}
