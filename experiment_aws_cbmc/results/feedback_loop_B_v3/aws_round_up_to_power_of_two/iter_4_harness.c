#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;

    // Assume n is within a reasonable range to avoid overflow in the function
    assume(n <= SIZE_MAX / 2 + 1);

    // Call function under test
    result = aws_round_up_to_power_of_two(n);

    // Postcondition: result is a power of two
    assert((result & (result - 1)) == 0);

    // Postcondition: result is the smallest power of two greater than or equal to n
    assert(result >= n);

    // Postcondition: if n is already a power of two, result should be equal to n
    if ((n & (n - 1)) == 0) {
        assert(result == n);
    }
}
