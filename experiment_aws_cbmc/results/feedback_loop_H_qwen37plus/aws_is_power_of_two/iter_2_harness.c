#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_is_power_of_two_harness() {
    size_t x = nondet_size_t();
    bool result = aws_is_power_of_two(x);
    if (result) {
        assert(x > 0);
        assert((x & (x - 1)) == 0);
    } else {
        assert(x == 0 || (x & (x - 1)) != 0);
    }
}
