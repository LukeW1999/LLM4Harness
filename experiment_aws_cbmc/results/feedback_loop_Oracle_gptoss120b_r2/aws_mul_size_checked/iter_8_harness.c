#include <aws/common/common.h>
#include <aws/common/math.h>
#include <limits.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a <= 100);
    __CPROVER_assume(b <= 100);

    size_t r;

    int ret = aws_mul_size_checked(a, b, &r);

    if (ret == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(!(a == 0 || b == 0 || a <= SIZE_MAX / b));
    }
}
