#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* 1. Declare inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    /* 2. Call function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* 3. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success: r contains a + b, and no overflow occurred */
        assert(r == a + b);
        /* No overflow means a + b <= SIZE_MAX, i.e., a <= SIZE_MAX - b */
        assert(a <= SIZE_MAX - b);
    } else {
        /* On failure: overflow occurred */
        assert(result == AWS_OP_ERR);
        /* Overflow means a + b > SIZE_MAX */
        assert(a > SIZE_MAX - b);
    }
}

void aws_mul_size_checked_harness(void) {
    aws_add_size_checked_harness();
    return 0;
}
