#include <aws/common/math.h>
#include <stdint.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Bound the inputs to avoid state space explosion */
    __CPROVER_assume(a <= 0xFF);
    __CPROVER_assume(b <= 0xFF);

    size_t result = aws_mul_size_saturating(a, b);

    /* Property 1: If either operand is 0, result must be 0 */
    if (a == 0 || b == 0) {
        assert(result == 0);
    }

    /* Property 2: Check overflow detection */
    if (a != 0 && b != 0) {
        if (a <= SIZE_MAX / b) {
            assert(result == a * b);
        } else {
            assert(result == SIZE_MAX);
        }
    }

    /* Property 3: Result is never greater than SIZE_MAX */
    assert(result <= SIZE_MAX);
}
