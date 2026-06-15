#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <aws/common/math.h>

/* CBMC provides nondeterministic generators for primitive types */
size_t nondet_size_t(void);

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = 0;

    bool overflow = aws_mul_size_saturating(a, b, &result);

    if (a == 0 || b == 0) {
        /* No overflow, result must be zero */
        assert(!overflow);
        assert(result == 0);
    } else if (a > SIZE_MAX / b) {
        /* Overflow case: result saturated to SIZE_MAX */
        assert(overflow);
        assert(result == SIZE_MAX);
    } else {
        /* Normal multiplication without overflow */
        assert(!overflow);
        assert(result == a * b);
    }
}
