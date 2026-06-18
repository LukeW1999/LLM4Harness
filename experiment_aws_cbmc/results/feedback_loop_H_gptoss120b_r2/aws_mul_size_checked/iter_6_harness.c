#include <assert.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = nondet_size_t();
    size_t old_result = result;

    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);
    __CPROVER_assume(result <= 1000);

    int ret = aws_mul_size_checked(a, b, &result);

    size_t max = (size_t)-1;

    if (a == 0) {
        assert(ret == 0);
        assert(result == 0);
    } else if (b <= max / a) {
        assert(ret == 0);
        assert(result == a * b);
    } else {
        assert(ret != 0);
        assert(result == old_result);
    }
}
