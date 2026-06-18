#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    size_t result;
    int ret = aws_mul_size_saturating(a, b, &result);

    if (ret == 0) {
        if (a != 0 && b != 0) {
            __CPROVER_assert(result / a == b, "multiplication check");
        } else {
            __CPROVER_assert(result == 0, "zero result check");
        }
    } else {
        __CPROVER_assert(result == (size_t)-1, "saturation check");
    }
}
