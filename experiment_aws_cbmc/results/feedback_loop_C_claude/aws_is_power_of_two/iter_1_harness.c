// === STEP 1: SUCCESS PATH ===
// aws_is_power_of_two is a boolean function (returns bool), not AWS_OP_SUCCESS/ERR
// When aws_is_power_of_two returns true:
//   - x != 0 AND (x & (x-1)) == 0
//   - No output parameters, pure function
//
// === STEP 2: FAILURE PATH ===
// When aws_is_power_of_two returns false:
//   - x == 0 OR (x & (x-1)) != 0
//   - No output parameters, pure function
//
// === STEP 3: FRAME CONDITIONS ===
// No struct parameters. Only a single size_t input x.
// The function is pure - no side effects, no memory modifications.
//
// === STEP 4: VALIDITY INVARIANTS ===
// No struct validity invariants needed.
// Key properties to verify:
//   - If result is true, then x != 0 and (x & (x-1)) == 0
//   - If result is false, then x == 0 or (x & (x-1)) != 0
//   - Result is consistent with the mathematical definition of power of two
//   - Special case: x=1 should return true (2^0)
//   - Special case: x=0 should return false

#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_is_power_of_two_harness(void) {
    // Create unconstrained input
    size_t x = nondet_size_t();
    
    // Call the function under test
    bool result = aws_is_power_of_two(x);
    
    // === Verify correctness of the result ===
    
    // If result is true, x must be non-zero and have exactly one bit set
    if (result) {
        assert(x != 0);
        assert((x & (x - 1)) == 0);
    }
    
    // If x is zero, result must be false
    if (x == 0) {
        assert(result == false);
    }
    
    // If x is non-zero and has more than one bit set, result must be false
    if (x != 0 && (x & (x - 1)) != 0) {
        assert(result == false);
    }
    
    // If x is non-zero and has exactly one bit set, result must be true
    if (x != 0 && (x & (x - 1)) == 0) {
        assert(result == true);
    }
    
    // Verify the result is exactly equivalent to the mathematical definition
    assert(result == (x != 0 && (x & (x - 1)) == 0));
    
    // Verify specific known values
    // x=1 is 2^0, should be power of two
    if (x == 1) {
        assert(result == true);
    }
    
    // x=2 is 2^1, should be power of two
    if (x == 2) {
        assert(result == true);
    }
    
    // x=3 is not a power of two
    if (x == 3) {
        assert(result == false);
    }
    
    // SIZE_MAX_POWER_OF_TWO should be a power of two
    // SIZE_MAX_POWER_OF_TWO = ((size_t)1) << (SIZE_BITS - 1)
    size_t max_pow2 = ((size_t)1) << (sizeof(size_t) * 8 - 1);
    if (x == max_pow2) {
        assert(result == true);
    }
}
