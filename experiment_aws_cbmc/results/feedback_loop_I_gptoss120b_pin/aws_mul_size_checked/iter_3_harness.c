#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_u64_saturating_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    uint64_t result = aws_mul_u64_saturating(a, b);

    bool overflow = (a != 0 && b > UINT64_MAX / a);
    if (overflow) {
        assert(result == UINT64_MAX);
    } else {
        assert(result == a * b);
    }
}
