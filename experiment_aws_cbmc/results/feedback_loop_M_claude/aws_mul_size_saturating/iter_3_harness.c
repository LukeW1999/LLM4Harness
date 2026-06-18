#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>

void harness(void) {
    size_t a;
    size_t b;

    size_t result = aws_mul_size_saturating(a, b);

    /* If no overflow, result == a * b */
    /* If overflow, result == SIZE_MAX */
    __CPROVER_assert(result == SIZE_MAX || result == a * b, "result is either SIZE_MAX or a*b");

    if (a != 0 && b != 0) {
        if (result != SIZE_MAX) {
            __CPROVER_assert(result == a * b, "no overflow means result equals a*b");
            __CPROVER_assert(result / a == b, "division check");
        }
    }

    if (a == 0 || b == 0) {
        __CPROVER_assert(result == 0, "zero operand means zero result");
    }
}
