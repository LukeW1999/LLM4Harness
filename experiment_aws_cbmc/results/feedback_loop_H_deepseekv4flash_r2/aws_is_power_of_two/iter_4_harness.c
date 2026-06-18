#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>

void aws_is_power_of_two_harness() {
    size_t n = nondet_size_t();
    bool result = aws_is_power_of_two(n);
    // A power of two is a positive integer with exactly one bit set.
    // (n > 0) is required because 0 is not a power of two.
    // The expression (n & (n - 1)) == 0 is true exactly when n is a power of two.
    if (n > 0 && (n & (n - 1)) == 0) {
        assert(result);
    } else {
        assert(!result);
    }
}
