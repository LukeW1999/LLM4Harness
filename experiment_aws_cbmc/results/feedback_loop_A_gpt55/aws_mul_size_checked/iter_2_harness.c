#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = nondet_size_t();
    size_t old_result = result;

    int overflow = (a != 0 && b > SIZE_MAX / a);

    int rv = aws_mul_size_checked(a, b, &result);

    assert(rv == AWS_OP_SUCCESS || rv == AWS_OP_ERR);

    if (overflow) {
        assert(rv == AWS_OP_ERR);
        assert(result == old_result);
    } else {
        assert(rv == AWS_OP_SUCCESS);
        assert(result == a * b);
    }
}
