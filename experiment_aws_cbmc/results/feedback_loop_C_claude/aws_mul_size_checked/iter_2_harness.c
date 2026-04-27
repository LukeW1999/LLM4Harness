#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;

    // Allocate output
    size_t r;
    size_t old_r;
    r = old_r;

    // Call the function
    int result = aws_mul_size_checked(a, b, &r);

    // Determine if overflow would occur
    int would_overflow = (a != 0) && (b > (SIZE_MAX / a));

    if (result == AWS_OP_SUCCESS) {
        assert(!would_overflow);
        assert(r == a * b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(would_overflow);
        assert(r == old_r);
    }

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
