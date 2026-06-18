#include <aws/common/math.h>
#include <stdint.h>
#include <stddef.h>

void harness(void) {
    size_t a;
    size_t b;

    size_t result = aws_mul_size_saturating(a, b);

    if (a == 0 || b == 0) {
        __CPROVER_assert(result == 0, "zero operand means zero result");
    }

    if (a != 0 && b != 0) {
        if (result != SIZE_MAX) {
            __CPROVER_assert(result == a * b, "no overflow means result equals a*b");
        }
    }
}
