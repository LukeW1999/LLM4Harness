#include <aws/common/math.h>
#include <assert.h>
#include <stddef.h>

void aws_add_size_checked_harness() {
    size_t a, b;
    size_t result;
    int ret = aws_add_size_checked(a, b, &result);

    if (ret == AWS_OP_SUCCESS) {
        /* No overflow – result equals a + b */
        assert(a <= SIZE_MAX - b);
        assert(result == a + b);
    } else {
        /* Overflow detected */
        assert(ret == AWS_OP_ERR);
        assert(a > SIZE_MAX - b);
    }
}
