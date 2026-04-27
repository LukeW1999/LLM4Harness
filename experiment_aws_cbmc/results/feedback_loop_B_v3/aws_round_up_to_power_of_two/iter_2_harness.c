#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;

    // Call function under test
    result = aws_round_up_to_power_of_two(n);

    // Postcondition: result is a power of two
    assert((result & (result - 1)) == 0);

    // Postcondition: result is the smallest power of two greater than or equal to n
    assert(result >= n);
}
