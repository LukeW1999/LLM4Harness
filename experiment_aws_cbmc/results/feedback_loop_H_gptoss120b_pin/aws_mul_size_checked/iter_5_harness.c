#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    /* Limit the range of inputs to keep CBMC tractable */
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    int ret = aws_mul_size_checked(a, b, &result);

    bool overflow = (a != 0 && b > SIZE_MAX / a);

    if (overflow) {
        assert(ret != 0);
    } else {
        assert(ret == 0);
        assert(result == a * b);
    }
}
