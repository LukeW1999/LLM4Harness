#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <limits.h>

void aws_add_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();
    size_t result;
    int ret = aws_add_size_saturating(a, b, &result);

    if (a > SIZE_MAX - b) {
        assert(ret != 0);
        assert(result == SIZE_MAX);
    } else {
        assert(ret == 0);
        assert(result == a + b);
    }
}
