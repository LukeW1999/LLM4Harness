// === STEP 1: SUCCESS PATH ===
// aws_add_size_saturating(a, b) returns a + b when no overflow occurs
//   - return value: equals a + b (the mathematical sum)
//
// === STEP 2: FAILURE PATH ===
// When a + b overflows size_t:
//   - return value: SIZE_MAX (saturating behavior)
//
// === STEP 3: FRAME CONDITIONS ===
// This function takes two size_t values by value (no pointers to structs).
// No struct parameters - only scalar inputs and a scalar return value.
// The function does not modify any external state.
//
// === STEP 4: VALIDITY INVARIANTS ===
// No struct validity invariants apply here.
// Key properties to verify:
//   1. If a + b does not overflow, result == a + b
//   2. If a + b overflows, result == SIZE_MAX
//   3. Result is always >= a (since b >= 0 and saturating)
//   4. Result is always >= b (since a >= 0 and saturating)

#include <aws/common/math.h>
#include <stdint.h>
#include <assert.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_add_size_saturating(a, b);

    // Check overflow detection
    if (a > SIZE_MAX - b) {
        // Overflow case: result must be SIZE_MAX
        assert(result == SIZE_MAX);
    } else {
        // No overflow case: result must be exact sum
        assert(result == a + b);
    }

    // Saturation property: result is always >= each operand
    // (since b >= 0 for size_t, adding b to a can only increase or saturate)
    assert(result >= a || result == SIZE_MAX);
    assert(result >= b || result == SIZE_MAX);

    // More precise: result is always >= a (saturating means at least a)
    // When no overflow: result = a + b >= a (since b >= 0)
    // When overflow: result = SIZE_MAX >= a (since SIZE_MAX is max)
    assert(result >= a);
    assert(result >= b);
}
