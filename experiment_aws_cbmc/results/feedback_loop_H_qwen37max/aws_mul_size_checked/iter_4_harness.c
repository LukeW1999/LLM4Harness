#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_mul_size_checked_harness() {
    size_t a;
    size_t b;
    size_t r;

    __CPROVER_assume(a == 0 || a == 1 || a > SIZE_MAX / 2);
    __CPROVER_assume(b == 0 || b == 1 || b > SIZE_MAX / 2);

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    }
}
