#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;
    size_t *pResult = &result;

    int ret = aws_mul_size_saturating(a, b, pResult);

    if (ret == 0) {
        if (a != 0 && b != 0) {
            __CPROVER_assert(*pResult / a == b, "multiplication check");
        } else {
            __CPROVER_assert(*pResult == 0, "zero result check");
        }
    } else {
        __CPROVER_assert(*pResult == (size_t)-1, "saturation check");
    }
}
