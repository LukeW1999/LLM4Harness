#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_is_power_of_two_harness() {
    size_t n = nondet_size_t();
    bool result = aws_is_power_of_two(n);

    if (result) {
        // n is a power of two: n != 0 and (n & (n-1)) == 0
        assert(n != 0);
        assert((n & (n - 1)) == 0);
    } else {
        // n is not a power of two: either n == 0 or (n & (n-1)) != 0
        assert((n == 0) || ((n & (n - 1)) != 0));
    }
}
