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
        assert(result == SIZE_MAX);
    } else {
        /* No overflow: result should be exact sum */
        assert(result == a + b);
    }
}
