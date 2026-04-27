// === STEP 1: SUCCESS PATH ===
// When aws_mul_size_checked returns AWS_OP_SUCCESS:
//   - *r: CHANGES to a * b (the product)
//   - a: UNCHANGED (input parameter)
//   - b: UNCHANGED (input parameter)
//   - Condition: a * b does NOT overflow size_t
//
// === STEP 2: FAILURE PATH ===
// When aws_mul_size_checked returns AWS_OP_ERR:
//   - *r: UNCHANGED (not written, or implementation-defined; we check it's not modified)
//   - a: UNCHANGED
//   - b: UNCHANGED
//   - Condition: a * b WOULD overflow size_t
//
// === STEP 3: FRAME CONDITIONS ===
// Parameters are scalar (size_t a, size_t b, size_t *r):
//   - a: UNCHANGED always (passed by value)
//   - b: UNCHANGED always (passed by value)
//   - *r: CHANGED on success (set to a*b), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - r must be a valid non-null pointer (we allocate it)
//   - On success: *r == a * b (no overflow occurred)
//   - On success: return value == AWS_OP_SUCCESS (0)
//   - On failure: return value == AWS_OP_ERR (-1)
//   - Overflow detection: if a != 0 && (SIZE_MAX / a) < b => overflow

#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    // === STEP 1: SUCCESS PATH ===
    // When aws_mul_size_checked returns AWS_OP_SUCCESS:
    //   - *r: CHANGES to a * b (the product)
    //   - a: UNCHANGED (input parameter, passed by value)
    //   - b: UNCHANGED (input parameter, passed by value)
    //
    // === STEP 2: FAILURE PATH ===
    // When aws_mul_size_checked returns AWS_OP_ERR:
    //   - *r: UNCHANGED (not written on overflow)
    //   - a: UNCHANGED
    //   - b: UNCHANGED
    //
    // === STEP 3: FRAME CONDITIONS ===
    //   - a: UNCHANGED always
    //   - b: UNCHANGED always
    //   - *r: CHANGED on success, UNCHANGED on failure
    //
    // === STEP 4: VALIDITY INVARIANTS ===
    //   - r != NULL (valid pointer)
    //   - On success: *r == a * b
    //   - On success: return == AWS_OP_SUCCESS
    //   - On failure: return == AWS_OP_ERR

    // Nondeterministic inputs
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    // Allocate output
    size_t r;
    size_t old_r = nondet_size_t(); // save old value
    r = old_r;

    // Call the function
    int result = aws_mul_size_checked(a, b, &r);

    // Determine if overflow would occur
    // Overflow if: a != 0 && b > SIZE_MAX / a
    int would_overflow = (a != 0) && (b > (SIZE_MAX / a));

    if (result == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(!would_overflow);
        assert(r == a * b);
        assert(result == 0); // AWS_OP_SUCCESS
    } else {
        // Failure path assertions
        assert(result == -1); // AWS_OP_ERR
        assert(would_overflow);
        // *r should be unchanged on failure
        assert(r == old_r);
    }

    // Return value must be either success or error
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
