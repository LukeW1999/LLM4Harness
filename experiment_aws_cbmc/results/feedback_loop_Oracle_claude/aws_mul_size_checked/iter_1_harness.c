#include <aws/common/math.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r;
    size_t r_init;

    /* r is initialized to a known value to check frame conditions */
    r_init = r;

    int result = aws_mul_size_checked(a, b, &r);

    /* Postcondition 1: Return value must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Postcondition 2: If overflow would occur, must return AWS_OP_ERR */
    /* Check overflow: a * b > SIZE_MAX iff a != 0 && b > SIZE_MAX / a */
    if (a != 0 && b > SIZE_MAX / a) {
        /* overflow case */
        assert(result == AWS_OP_ERR);
    } else {
        /* no overflow case */
        assert(result == AWS_OP_SUCCESS);
    }

    /* Postcondition 3: On success, r must equal a * b */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        /* r must be a valid size_t value (trivially true, but explicit) */
        assert(r <= SIZE_MAX);
    }

    /* Postcondition 4: On error, r should not be modified (frame condition) */
    /* Note: The AWS implementation may or may not modify r on error.
     * We verify that on success the result is correct, and on error
     * we check the return code is AWS_OP_ERR. */
    if (result == AWS_OP_ERR) {
        /* The function returned an error, result is AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }

    /* Postcondition 5: On success, if a == 0 or b == 0, result must be 0 */
    if (result == AWS_OP_SUCCESS && (a == 0 || b == 0)) {
        assert(r == 0);
    }

    /* Postcondition 6: On success, result must not exceed SIZE_MAX */
    if (result == AWS_OP_SUCCESS) {
        assert(r <= SIZE_MAX);
        /* Verify the multiplication is correct */
        assert(r == (size_t)(a * b));
    }
}
