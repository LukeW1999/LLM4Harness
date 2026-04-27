// === STEP 1: SUCCESS PATH ===
// When aws_mul_size_checked returns AWS_OP_SUCCESS:
//   - *r: CHANGES to the product of a and b, unless overflow occurs, in which case it is set to SIZE_MAX

// === STEP 2: FAILURE PATH ===
// When aws_mul_size_checked returns AWS_OP_ERR:
//   - *r: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY parameter, mark CHANGED or UNCHANGED:
//   a (size_t): UNCHANGED always
//   b (size_t): UNCHANGED always
//   r (size_t*):
//     - *r: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - No specific validity invariants mentioned for size_t parameters or pointers

#include <aws/common/math.h>
#include <stddef.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r_old;
    size_t r_new;
    int result;

    // Capture the old value of r
    r_old = nondet_size_t();
    r_new = r_old;

    // Call the function under test
    result = aws_mul_size_checked(a, b, &r_new);

    if (result == AWS_OP_SUCCESS) {
        // Success path
        assert(r_new == (a <= SIZE_MAX / b ? a * b : SIZE_MAX));
    } else {
        // Failure path
        assert(result == AWS_OP_ERR);
        assert(r_new == r_old);
    }
}
