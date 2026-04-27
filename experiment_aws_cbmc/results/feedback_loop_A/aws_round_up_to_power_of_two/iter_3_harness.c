#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    /* 1. Declare and bound data structures */
    size_t x = nondet_size_t();
    __CPROVER_assume(x <= SIZE_MAX / 2); // Ensure x is within a reasonable range to avoid overflow

    /* 2. Call function under test */
    size_t result = aws_round_up_to_power_of_two(x);

    /* 3. Assert postconditions for BOTH success and failure paths */
    /* For this function, we expect it to always succeed and return a power of two */
    assert((result & (result - 1)) == 0); // Check if result is a power of two
    assert(result >= x); // Check if result is at least x
    if (x != 0) {
        assert(result / x <= 2); // Check if result is the smallest power of two greater than or equal to x
    }
}
