// === STEP 1: SUCCESS PATH ===
// aws_mul_size_saturating(a, b) returns a * b when no overflow occurs.
// This is a pure function with no struct parameters - it takes two size_t values
// and returns a size_t result.
// On no overflow: returns a * b
//
// === STEP 2: FAILURE PATH ===
// When overflow occurs: returns SIZE_MAX (saturating behavior)
// No struct parameters to track.
//
// === STEP 3: FRAME CONDITIONS ===
// No struct parameters. The function only takes two size_t values (a, b) by value
// and returns a size_t. No memory is modified.
// a: UNCHANGED (passed by value)
// b: UNCHANGED (passed by value)
//
// === STEP 4: VALIDITY INVARIANTS ===
// No struct validity invariants needed.
// Key properties to verify:
// 1. If a == 0 || b == 0: result == 0
// 2. If no overflow (a * b fits in size_t): result == a * b
// 3. If overflow: result == SIZE_MAX
// 4. Result is always >= 0 (trivially true for size_t)
// 5. If result != SIZE_MAX, then a * b == result (no overflow occurred)

#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    // Property 1: If either operand is 0, result must be 0
    if (a == 0 || b == 0) {
        assert(result == 0);
    }

    // Property 2: Check overflow detection
    // If a != 0 and b != 0, check whether overflow would occur
    if (a != 0 && b != 0) {
        if (a <= SIZE_MAX / b) {
            // No overflow: result should equal a * b
            assert(result == a * b);
        } else {
            // Overflow: result should be SIZE_MAX
            assert(result == SIZE_MAX);
        }
    }

    // Property 3: Result is never greater than SIZE_MAX (trivially true for size_t,
    // but let's verify the saturating bound explicitly)
    assert(result <= SIZE_MAX);

    // Property 4: If result is not SIZE_MAX, then a * b == result
    // (i.e., no overflow occurred and the multiplication is exact)
    // This is implied by property 2 but we add it for clarity
    if (result != SIZE_MAX) {
        // Either a or b is 0, or no overflow occurred
        assert(a == 0 || b == 0 || (a <= SIZE_MAX / b && result == a * b));
    }

    // Property 5: Commutativity - aws_mul_size_saturating(a, b) == aws_mul_size_saturating(b, a)
    size_t result2 = aws_mul_size_saturating(b, a);
    assert(result == result2);
}
