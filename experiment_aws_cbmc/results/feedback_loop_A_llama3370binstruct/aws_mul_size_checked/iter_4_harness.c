#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_checked_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;
    int res = aws_mul_size_checked(a, b, &result);

    if (res == AWS_OP_SUCCESS) {
        assert(result == a * b);
        assert(a == a); /* a is unchanged */
        assert(b == b); /* b is unchanged */
    } else {
        assert(res == AWS_OP_ERR_OVERFLOW);
        assert(a * b > SIZE_MAX);
        assert(a == a); /* a is unchanged */
        assert(b == b); /* b is unchanged */
    }

    assert(res == AWS_OP_SUCCESS || res == AWS_OP_ERR_OVERFLOW);
    assert(a <= SIZE_MAX);
    assert(b <= SIZE_MAX);
}
