#include <assert.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;
    int ret = aws_mul_size_saturating(a, b, &result);

    if (a != 0 && b > SIZE_MAX / a) {
        assert(ret != 0);
        assert(result == SIZE_MAX);
    } else {
        assert(ret == 0);
        assert(result == a * b);
    }
}
