#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r = 0;

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* Verify no overflow occurred and result is correct */
        assert(r == a * b);
        /* Also verify that overflow didn't actually happen */
        if (a != 0) {
            assert(r / a == b);
        }
    } else {
        assert(result == AWS_OP_ERR);
        /* Overflow must have occurred */
        if (a != 0 && b != 0) {
            assert(a > SIZE_MAX / b || b > SIZE_MAX / a);
        }
    }
}
