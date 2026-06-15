#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <aws/common/math.h>

extern size_t nondet_size_t(void);

int main(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

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
