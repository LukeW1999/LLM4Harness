// === STEP 1: SUCCESS PATH ===
// When aws_add_size_saturating returns AWS_OP_SUCCESS (or the successful value):
//   - The result is the sum of a and b if no overflow occurs

// === STEP 2: FAILURE PATH ===
// When aws_add_size_saturating returns SIZE_MAX (or fails):
//   - The result is SIZE_MAX if overflow occurs

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   There are no struct parameters in this function. The function takes two size_t parameters and returns a size_t value.
//   Therefore, there are no fields to track for changes.

// === STEP 4: VALIDITY INVARIANTS ===
//   - There are no struct parameters to validate.

#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = aws_add_size_saturating(a, b);

    if (result != SIZE_MAX) {
        // Success path
        assert(result == a + b);
    } else {
        // Failure path due to overflow
        assert(a + b < a || a + b < b); // Overflow condition
    }
}
