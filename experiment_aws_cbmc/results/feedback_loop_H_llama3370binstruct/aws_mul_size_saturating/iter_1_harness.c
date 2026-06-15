#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(a * b > SIZE_MAX);
    }

    assert(a == a); /* a is unchanged */
    assert(b == b); /* b is unchanged */
}
