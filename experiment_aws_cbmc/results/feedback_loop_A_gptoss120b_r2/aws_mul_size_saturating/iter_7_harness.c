#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t old_a = a;
    size_t old_b = b;

    size_t result;

    int ret = aws_mul_size_saturating(a, b, &result);

    __CPROVER_assert(a == old_a, "a unchanged");
    __CPROVER_assert(b == old_b, "b unchanged");

    if (ret == 0) {
        __CPROVER_assert(result == a * b, "result equals product");
    } else {
        __CPROVER_assert(result == (size_t)-1, "result indicates overflow");
    }
}
