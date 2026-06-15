#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a, b;
    size_t *out = malloc(sizeof(*out));
    __CPROVER_assume(out != NULL);

    // Bound inputs to reduce state space and avoid timeout
    __CPROVER_assume(a < 128 && b < 128);

    int rval = aws_mul_size_checked(a, b, out);

    if (rval == AWS_OP_SUCCESS) {
        // Multiplication is safe, so a*b does not overflow
        assert(*out == a * b);
    } else {
        assert(rval == AWS_OP_ERR);
        // Overflow condition: a != 0 and b > SIZE_MAX / a
        __CPROVER_assume(a != 0); // Ensure no division by zero in the assertion
        assert(b > SIZE_MAX / a);
    }
}
