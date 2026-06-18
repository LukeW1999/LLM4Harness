#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <limits.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;
    int ret = aws_add_size_saturating(a, b, &result);

    if (a <= SIZE_MAX - b) {
        __CPROVER_assert(ret == 0, "no overflow");
        __CPROVER_assert(result == a + b, "correct sum");
    } else {
        __CPROVER_assert(ret != 0, "overflow");
        __CPROVER_assert(result == SIZE_MAX, "saturated");
    }
}
