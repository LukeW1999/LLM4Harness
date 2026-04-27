// === STEP 1: SUCCESS PATH ===
// When aws_mul_size_saturating returns AWS_OP_SUCCESS (or the successful value):
//   - The function returns the result of a * b
//   - No changes to any parameters as it's a pure function

// === STEP 2: FAILURE PATH ===
// When aws_mul_size_saturating returns AWS_OP_ERR (or fails):
//   - The function returns SIZE_MAX
//   - No changes to any parameters as it's a pure function

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   There are no struct parameters, only primitive types size_t a, size_t b
//   No fields to list as there are no structs

// === STEP 4: VALIDITY INVARIANTS ===
//   - No validity invariants as there are no structs

#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    // Capture old values of inputs (though they should remain unchanged)
    size_t old_a = a;
    size_t old_b = b;

    result = aws_mul_size_saturating(a, b);

    // Check that inputs are unchanged
    assert(a == old_a);
    assert(b == old_b);

    // Check the result based on overflow condition
    if (__builtin_mul_overflow(a, b, &result)) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
