#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <aws/common/math.h>

extern size_t nondet_size_t(void);

int main(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = 0;
    bool overflow = aws_mul_size_saturating(a, b, &result);

    /* Check that overflow flag matches the mathematical condition */
    bool expected_overflow = (a != 0 && b != 0 && a > SIZE_MAX / b);
    assert(overflow == expected_overflow);

    /* Verify the saturated result */
    if (overflow) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }

    return 0;
}
