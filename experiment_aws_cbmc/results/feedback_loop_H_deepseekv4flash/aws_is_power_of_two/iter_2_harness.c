#include <aws/common/math.h>
#include <stdbool.h>
#include <stddef.h>

void aws_is_power_of_two_harness() {
    size_t n = nondet_size_t();
    bool rval = aws_is_power_of_two(n);
    if (rval) {
        assert(n != 0);
        assert((n & (n - 1)) == 0);
    } else {
        assert(n == 0 || (n & (n - 1)) != 0);
    }
}
