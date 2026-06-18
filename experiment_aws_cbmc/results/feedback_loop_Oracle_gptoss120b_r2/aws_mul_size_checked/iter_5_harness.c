#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    size_t r;

    int ret = aws_mul_size_checked(a, b, &r);

    if (ret == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(!(a == 0 || b == 0 || a <= SIZE_MAX / b));
    }
}
