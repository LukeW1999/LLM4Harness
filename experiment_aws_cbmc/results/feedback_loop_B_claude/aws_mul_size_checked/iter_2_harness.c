#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 64
#endif

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t result;

    /* Non-deterministic inputs - no constraints, let CBMC explore all values */
    int ret = aws_mul_size_checked(a, b, &result);

    /* Postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* On success, result must equal a * b and must not have overflowed */
        assert(result == a * b);
        /* Verify no overflow occurred: if a != 0, then result / a == b */
        if (a != 0) {
            assert(result / a == b);
        }
        if (b != 0) {
            assert(result / b == a);
        }
    } else {
        /* On failure (overflow), the multiplication would have overflowed */
        /* Verify that overflow actually would occur */
        /* If a != 0 and b != 0, then a * b overflowed SIZE_MAX */
        assert(ret == AWS_OP_ERR);
        if (a != 0 && b != 0) {
            /* Overflow condition: b > SIZE_MAX / a */
            assert(b > SIZE_MAX / a);
        }
    }
}
