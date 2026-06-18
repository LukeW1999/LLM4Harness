#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

size_t __CPROVER_nondet_size_t(void);

void aws_mul_size_saturating_harness(void) {
    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();

    size_t old_result = __CPROVER_nondet_size_t();
    size_t result = old_result;

    int ret = aws_mul_size_saturating(a, b, &result);

    if (ret == 0) {
        __CPROVER_assert(result == a * b, "result equals product");
        if (a != 0) {
            __CPROVER_assert(result / a == b, "division yields b");
        }
    } else {
        __CPROVER_assert(result == old_result, "result unchanged on overflow");
        __CPROVER_assert(a != 0 && b != 0 && ((size_t)-1 / a) < b, "overflow condition holds");
    }
}
