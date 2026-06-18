#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <assert.h>

void aws_is_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    size_t old_n = n;

    bool result = aws_is_power_of_two(n);

    if (result) {
        assert(n != 0);
        assert((n & (n - 1)) == 0);
    }

    assert(n == old_n);
}
