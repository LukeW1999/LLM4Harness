#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;
    int res = aws_mul_size_checked(a, b, &result);

    if (res == AWS_OP_SUCCESS) {
        assert(result == a * b);
    } else {
        assert(res == AWS_OP_ERR_OVERFLOW);
        assert(a * b > SIZE_MAX);
    }

    assert(a == a); /* a is unchanged */
    assert(b == b); /* b is unchanged */
}
