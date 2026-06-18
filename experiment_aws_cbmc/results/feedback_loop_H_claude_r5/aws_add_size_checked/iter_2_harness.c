#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_harness() {
    size_t a;
    size_t b;
    size_t r;

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
        assert(b == 0 || a <= SIZE_MAX - b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(b > 0 && a > SIZE_MAX - b);
    }
}
