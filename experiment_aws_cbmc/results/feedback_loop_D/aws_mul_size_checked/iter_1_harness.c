// === STEP 1: SUCCESS PATH ===
// When aws_mul_size_checked returns AWS_OP_SUCCESS (or the successful value):
//   - *r: CHANGES to the result of a * b

// === STEP 2: FAILURE PATH ===
// When aws_mul_size_checked returns AWS_OP_ERR (or fails):
//   - *r: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   r (pointer to size_t):
//     - value: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - No structs are passed as parameters, so no validity invariants to check

#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stddef.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r_old;
    size_t r_new;
    int result;

    // Save old state
    r_old = nondet_size_t();

    // Call the function
    result = aws_mul_size_checked(a, b, &r_new);

    if (result == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(r_new == (a <= SIZE_MAX / b ? a * b : SIZE_MAX));
    } else {
        // Failure path assertions
        assert(result == AWS_OP_ERR);
        assert(r_new == r_old); // Ensure r is unchanged on failure
    }
}
