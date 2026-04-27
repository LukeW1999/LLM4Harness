#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stddef.h>

void aws_add_size_saturating_harness() {
    /* 1. Declare and bound data structures */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    /* 2. Call function under test */
    r = aws_add_size_saturating(a, b);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (a > SIZE_MAX - b) {
        assert(r == SIZE_MAX);  // Check for saturation
    } else {
        assert(r == a + b);     // Check for normal addition
    }

    /* 4. Assert fields that must NOT change regardless of result */
    // No other fields to assert as this function only uses primitive types

    /* 5. Assert validity invariants always holds */
    // No complex structure to validate
}
