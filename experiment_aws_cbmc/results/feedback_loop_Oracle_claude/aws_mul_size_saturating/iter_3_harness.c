#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Bound the inputs to avoid state space explosion */
    __CPROVER_assume(a <= 0xFFFF);
    __CPROVER_assume(b <= 0xFFFF);

    /* Call the function under test */
    size_t result = aws_mul_size_saturating(a, b);

    /* Postcondition: Result is always bounded by SIZE_MAX */
    assert(result <= SIZE_MAX);

    /* Postcondition: If either operand is 0, result must be 0 */
    if (a == 0 || b == 0) {
        assert(result == 0);
    }

    /* Postcondition: If a == 1, result must be b */
    if (a == 1) {
        assert(result == b);
    }

    /* Postcondition: If b == 1, result must be a */
    if (b == 1) {
        assert(result == a);
    }

    /* Postcondition: result is either SIZE_MAX or the exact product */
    assert(result == SIZE_MAX || result == a * b);
}
