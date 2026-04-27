#include <aws/common/math.h>
#include <assert.h>

void aws_is_power_of_two_harness() {
    size_t n = nondet_size_t();
    bool result;

    result = aws_is_power_of_two(n);

    if (n == 0) {
        assert(!result);
    } else {
        assert(result == ((n & (n - 1)) == 0));
    }
}
