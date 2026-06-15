#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/common.h>
#include <aws/common/math.h>

void aws_mul_size_saturating_harness(void) {
    /* nondeterministic inputs */
    size_t a;
    size_t b;

    /* call the function under test */
    size_t result = aws_mul_size_saturating(a, b);

    /* compute the expected result according to the contract */
    bool overflow = (a != 0 && b > SIZE_MAX / a);
    size_t expected = overflow ? SIZE_MAX : a * b;

    /* post‑condition: result must match the contract */
    assert(result == expected);

    return 0;
}
