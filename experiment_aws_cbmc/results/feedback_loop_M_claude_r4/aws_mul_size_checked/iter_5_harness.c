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

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        /* Verify no overflow occurred */
        if (a != 0) {
            assert(r / a == b);
        }
    } else {
        assert(result == AWS_OP_ERR);
        /* Overflow must have occurred */
        assert(a != 0 && b > SIZE_MAX / a);
    }
}
