#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    int ret = aws_mul_size_saturating(a, b, &result);

    if (a == 0 || b == 0) {
        assert(ret == 0);
        assert(result == 0);
    } else if (a > SIZE_MAX / b) {
        assert(ret != 0);
        assert(result == SIZE_MAX);
    } else {
        assert(ret == 0);
        assert(result == a * b);
    }
}
