#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void harness(void) {
    size_t a;
    size_t b;
    size_t r;

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        if (a != 0) {
            assert(b <= SIZE_MAX / a);
        }
    } else {
        if (a != 0) {
            assert(b > SIZE_MAX / a);
        }
    }
}
