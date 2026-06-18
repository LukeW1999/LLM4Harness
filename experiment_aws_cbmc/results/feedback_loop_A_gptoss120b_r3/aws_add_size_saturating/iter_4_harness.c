#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_add_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    size_t result;

    int rc = aws_add_size_saturating(a, b, &result);

    if (rc == 0) {
        assert(result == a + b);
        assert(a <= SIZE_MAX - b);
    } else {
        assert(result == SIZE_MAX);
        assert(a > SIZE_MAX - b);
    }
}
