#include <aws/common/math.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

size_t nondet_size_t(void);

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_add_size_saturating(a, b);

    /* If no overflow, result equals a + b */
    if (a <= SIZE_MAX - b) {
        assert(result == a + b);
    } else {
        /* On overflow, result is saturated to SIZE_MAX */
        assert(result == SIZE_MAX);
    }

    /* Result is always >= both inputs */
    assert(result >= a);
    assert(result >= b);

    /* Result never exceeds SIZE_MAX */
    assert(result <= SIZE_MAX);
}
