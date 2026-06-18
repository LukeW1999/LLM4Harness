#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t old_a = a;
    size_t old_b = b;

    size_t out;
    bool overflow = aws_add_size_saturating(a, b, &out);

    if (overflow) {
        __CPROVER_assert(out == SIZE_MAX, "out should be SIZE_MAX on overflow");
        __CPROVER_assert(SIZE_MAX - a < b, "overflow condition holds");
    } else {
        __CPROVER_assert(out == a + b, "out should be a + b when no overflow");
        __CPROVER_assert(SIZE_MAX - a >= b, "no overflow condition holds");
    }

    __CPROVER_assert(a == old_a, "a unchanged");
    __CPROVER_assert(b == old_b, "b unchanged");
}
