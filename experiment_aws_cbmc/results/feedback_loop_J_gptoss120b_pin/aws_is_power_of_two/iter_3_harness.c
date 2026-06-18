#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_is_power_of_two_harness() {
    size_t n = nondet_size_t();

    bool result = aws_is_power_of_two(n);

    if (result) {
        assert(n != 0);
        assert((n & (n - 1)) == 0);
    }
}
