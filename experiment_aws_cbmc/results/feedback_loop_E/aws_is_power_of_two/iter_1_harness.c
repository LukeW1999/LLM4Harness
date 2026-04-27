#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_is_power_of_two_harness() {
    size_t x = nondet_size_t();

    bool result = aws_is_power_of_two(x);

    if (result) {
        // If the result is true, x must be a power of two
        assert(x > 0);
        assert((x & (x - 1)) == 0);
    } else {
        // If the result is false, x is not a power of two
        assert((x == 0) || (x & (x - 1)) != 0);
    }
}
