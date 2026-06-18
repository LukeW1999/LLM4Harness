#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_is_power_of_two_harness() {
    size_t n = nondet_size_t();
    bool result = aws_is_power_of_two(n);
    
    if (n == 0) {
        assert(!result);
    } else {
        assert(result == ((n & (n - 1)) == 0));
    }
}
