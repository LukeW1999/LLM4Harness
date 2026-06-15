#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <aws/common/math.h>

extern size_t nondet_size_t(void);
extern _Bool nondet_bool(void);
extern void __CPROVER_assume(_Bool condition);

int main(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    _Bool test_overflow = nondet_bool();

    if (test_overflow) {
        /* Force an overflow condition */
        __CPROVER_assume(a != 0 && b != 0 && a > SIZE_MAX / b);
    } else {
        /* Force a non‑overflow condition */
        __CPROVER_assume(b == 0 || a <= SIZE_MAX / b);
    }

    size_t result = 0;
    bool overflow = aws_mul_size_saturating(a, b, &result);

    if (overflow) {
        /* Overflow case: result must be saturated to SIZE_MAX */
        assert(result == SIZE_MAX);
    } else {
        /* No overflow: multiplication must be exact */
        assert(b == 0 || a <= SIZE_MAX / b);
        assert(result == a * b);
    }

    return 0;
}
