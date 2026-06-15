#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <aws/common/math.h>

extern size_t nondet_size_t(void);

int main(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;
    bool overflow = aws_mul_size_saturating(a, b, &result);

    bool expected_overflow = (a != 0 && b != 0 && a > SIZE_MAX / b);
    assert(overflow == expected_overflow);
    assert(result == (overflow ? SIZE_MAX : a * b));

    return 0;
}
