#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>

void aws_mul_size_saturating_harness() {
    size_t a;
    size_t b;

    __CPROVER_assume(a <= 65536);
    __CPROVER_assume(b <= 65536);

    size_t result = aws_mul_size_saturating(a, b);

    if (a == 0 || b == 0) {
        __CPROVER_assert(result == 0, "result is 0 when a or b is 0");
    } else if (a > SIZE_MAX / b) {
        __CPROVER_assert(result == SIZE_MAX, "result is SIZE_MAX on overflow");
    } else {
        __CPROVER_assert(result == a * b, "result is a * b when no overflow");
    }
}
