#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    // Nondeterministic inputs
    size_t a;
    size_t b;
    size_t r;
    size_t old_r = r; // save old value of r

    // Call the function under test
    int result = aws_mul_size_checked(a, b, &r);

    // Result must be either AWS_OP_SUCCESS or AWS_OP_ERR
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        // On success, r holds the correct product
        assert(r == a * b);
        // Verify no overflow occurred
        if (a != 0) {
            assert(b <= SIZE_MAX / a);
        }
    } else {
        // r should be unchanged on failure
        assert(r == old_r);
        // Overflow condition: a != 0 and b > SIZE_MAX / a
        if (a != 0) {
            assert(b > SIZE_MAX / a);
        } else {
            // If a == 0, multiplication can never overflow
            assert(0);
        }
    }
}
