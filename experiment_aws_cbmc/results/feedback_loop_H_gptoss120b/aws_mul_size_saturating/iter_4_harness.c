#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <aws/common/math.h>

/* Nondeterministic generator for size_t */
size_t nondet_size_t(void);

int main(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;
    bool overflow = aws_mul_size_saturating(a, b, &result);

    if (a == 0 || b == 0) {
        /* No overflow, result must be zero */
        assert(!overflow);
        assert(result == 0);
    } else {
        /* a != 0 && b != 0, safe to compute SIZE_MAX / b */
        if (a > SIZE_MAX / b) {
            /* Overflow case: result saturated to SIZE_MAX */
            assert(overflow);
            assert(result == SIZE_MAX);
        } else {
            /* Normal multiplication without overflow */
            assert(!overflow);
            assert(result == a * b);
        }
    }

    return 0;
}
