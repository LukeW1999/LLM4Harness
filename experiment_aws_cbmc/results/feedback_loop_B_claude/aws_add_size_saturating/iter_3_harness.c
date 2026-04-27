#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

size_t aws_add_size_saturating(size_t a, size_t b);

void aws_add_size_saturating_harness(void) {
    size_t a;
    size_t b;

    size_t result = aws_add_size_saturating(a, b);

    /* If addition would overflow, result should be SIZE_MAX */
    if (a > SIZE_MAX - b) {
        __CPROVER_assert(result == SIZE_MAX, "saturating result should be SIZE_MAX on overflow");
    } else {
        /* No overflow: result should be exact sum */
        __CPROVER_assert(result == a + b, "result should be exact sum when no overflow");
    }
}
