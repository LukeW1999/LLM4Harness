// === STEP 1: SUCCESS PATH ===
// aws_add_size_saturating(a, b) returns a + b when no overflow occurs
//   - return value: equals a + b (the mathematical sum)
//
// === STEP 2: FAILURE PATH (overflow) ===
// When a + b would overflow size_t:
//   - return value: SIZE_MAX (saturating behavior)
//
// === STEP 3: FRAME CONDITIONS ===
// This function takes two size_t values by value (no pointers to structs).
// No struct parameters - only scalar inputs and scalar return value.
// The function is pure: no side effects on any memory.
//
// === STEP 4: VALIDITY INVARIANTS ===
// The function returns either:
//   - a + b if a + b <= SIZE_MAX (no overflow)
//   - SIZE_MAX if a + b > SIZE_MAX (overflow/saturation)
// In both cases: result >= a (if b > 0), result >= b (if a > 0)
// result is always >= a when b > 0, and >= b when a > 0
// More precisely: result == (a + b) if no overflow, else SIZE_MAX

#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_add_size_saturating_harness(void) {
    // Non-deterministic inputs
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    // Call the function under test
    size_t result = aws_add_size_saturating(a, b);

    // === Encode Step 1 & 2 as assertions ===

    // Check overflow condition
    if (a > SIZE_MAX - b) {
        // Overflow case: result must be SIZE_MAX (saturating)
        assert(result == SIZE_MAX);
    } else {
        // No overflow case: result must equal a + b
        assert(result == a + b);
    }

    // === Additional sanity checks ===

    // Result is always >= a when b == 0 (result == a)
    // Result is always >= a when no overflow
    // Result is SIZE_MAX when overflow (which is >= a since SIZE_MAX is max)
    // So result >= a always holds when b > 0 or b == 0
    // Actually: if no overflow, result = a + b >= a. If overflow, result = SIZE_MAX >= a.
    assert(result >= a || b == 0); // result >= a always (b==0 means result==a, still >=)
    // More simply: result is always >= a
    assert(result >= a);
    assert(result >= b);

    // Result is bounded by SIZE_MAX
    assert(result <= SIZE_MAX);

    // If both a and b are 0, result must be 0
    if (a == 0 && b == 0) {
        assert(result == 0);
    }

    // If a == 0, result == b (no overflow possible from a side)
    if (a == 0) {
        assert(result == b);
    }

    // If b == 0, result == a
    if (b == 0) {
        assert(result == a);
    }
}
