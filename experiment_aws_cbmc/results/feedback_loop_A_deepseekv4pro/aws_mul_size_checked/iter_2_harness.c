#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = nondet_size_t();
    r = old_r;

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(size_t)));

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        assert((a == 0) || (b <= SIZE_MAX / a));
    } else {
        assert(r == old_r);
        assert(a > 0 && b > SIZE_MAX / a);
    }
}
