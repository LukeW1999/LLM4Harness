#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>

void harness(void) {
    size_t a;
    size_t b;

    size_t result = aws_mul_size_saturating(a, b);

    if (a == 0 || b == 0) {
        __CPROVER_assert(result == 0, "result is 0 when either operand is 0");
    } else if (a <= SIZE_MAX / b) {
        __CPROVER_assert(result == a * b, "result equals a*b when no overflow");
    } else {
        __CPROVER_assert(result == SIZE_MAX, "result is SIZE_MAX on overflow");
    }
}
