#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

uint64_t nondet_uint64_t(void);

void aws_is_power_of_two_harness(void) {
    size_t n = (size_t)nondet_uint64_t();

    int result = aws_is_power_of_two(n);

    if (n != 0 && (n & (n - 1)) == 0) {
        assert(result);
    } else {
        assert(!result);
    }
}
