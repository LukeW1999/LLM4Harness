// === STEP 1: SUCCESS PATH ===
// When aws_mul_size_checked returns AWS_OP_SUCCESS:
//   - *r: CHANGES to a * b (the product)
//   - return value: 0 (AWS_OP_SUCCESS)
//
// === STEP 2: FAILURE PATH ===
// When aws_mul_size_checked returns AWS_OP_ERR (overflow detected):
//   - *r: UNCHANGED (not written)
//   - return value: -1 (AWS_OP_ERR)
//
// === STEP 3: FRAME CONDITIONS ===
// Parameters a and b are passed by value, so they are never modified.
// r (size_t pointer):
//   - *r: CHANGED on success (set to a * b), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - r must be a valid non-null pointer (we allocate it)
//   - On success: *r == a * b and no overflow occurred
//   - On failure: overflow would have occurred (a != 0 && b > SIZE_MAX / a)

#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    // Nondeterministic inputs
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = r; // save old value of r

    // Call the function under test
    int result = aws_mul_size_checked(a, b, &r);

    // Result must be either AWS_OP_SUCCESS or AWS_OP_ERR
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        // === STEP 1: SUCCESS PATH assertions ===
        // No overflow: either a == 0, b == 0, or b <= SIZE_MAX / a
        assert(*(&r) == a * b);
        // Verify no overflow occurred
        if (a != 0) {
            assert(b <= SIZE_MAX / a);
        }
        // r holds the correct product
        assert(r == a * b);
    } else {
        // === STEP 2: FAILURE PATH assertions ===
        // result == AWS_OP_ERR means overflow was detected
        assert(result == AWS_OP_ERR);
        // r should be unchanged on failure
        assert(r == old_r);
        // Overflow condition: a != 0 and b > SIZE_MAX / a
        if (a != 0) {
            assert(b > SIZE_MAX / a);
        } else {
            // If a == 0, multiplication can never overflow, so this path shouldn't happen
            assert(0); // a == 0 should never cause overflow
        }
    }
}
