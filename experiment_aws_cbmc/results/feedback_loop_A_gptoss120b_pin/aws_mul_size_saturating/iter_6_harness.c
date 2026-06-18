#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness() {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();
    size_t result;
    int rc = aws_mul_size_saturating(a, b, &result);

    if (rc == 0) {
        if (a != 0) {
            assert(result / a == b);
        } else {
            assert(result == 0);
        }
        assert(b == 0 || a <= SIZE_MAX / b);
    } else {
        assert(!(b == 0 || a <= SIZE_MAX / b));
    }
}
