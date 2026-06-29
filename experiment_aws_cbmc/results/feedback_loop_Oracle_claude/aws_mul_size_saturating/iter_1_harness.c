#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Call the function under test */
    size_t result = aws_mul_size_saturating(a, b);

    /* Postcondition 1: Return value correctness
     * If the multiplication would overflow, result must be SIZE_MAX.
     * If it doesn't overflow, result must equal a * b.
     */

    /* Check overflow using 64-bit arithmetic when size_t is 32-bit,
     * or use __uint128_t when size_t is 64-bit */
#if SIZE_MAX == UINT32_MAX
    uint64_t wide = (uint64_t)a * (uint64_t)b;
    if (wide > (uint64_t)SIZE_MAX) {
        /* Overflow case: result must be SIZE_MAX */
        assert(result == SIZE_MAX);
    } else {
        /* No overflow: result must equal a * b */
        assert(result == (size_t)wide);
    }
#elif SIZE_MAX == UINT64_MAX
    /* Use __uint128_t for 64-bit size_t */
    __uint128_t wide128 = (__uint128_t)a * (__uint128_t)b;
    if (wide128 > (__uint128_t)SIZE_MAX) {
        /* Overflow case: result must be SIZE_MAX */
        assert(result == SIZE_MAX);
    } else {
        /* No overflow: result must equal a * b */
        assert(result == (size_t)wide128);
    }
#endif

    /* Postcondition 2: Result is always bounded by SIZE_MAX */
    assert(result <= SIZE_MAX);

    /* Postcondition 3: If either operand is 0, result must be 0 */
    if (a == 0 || b == 0) {
        assert(result == 0);
    }

    /* Postcondition 4: If both operands are 1, result must be 1 */
    if (a == 1 && b == 1) {
        assert(result == 1);
    }

    /* Postcondition 5: If a == 1, result must be b (no overflow possible from single operand) */
    if (a == 1) {
        assert(result == b);
    }

    /* Postcondition 6: If b == 1, result must be a */
    if (b == 1) {
        assert(result == a);
    }

    /* Postcondition 7: Saturation correctness - result is either SIZE_MAX or the exact product */
    assert(result == SIZE_MAX || result == a * b);
}
