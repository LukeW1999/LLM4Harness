// === STEP 1: SUCCESS PATH ===
// When aws_add_size_checked returns AWS_OP_SUCCESS (0):
//   - *r: CHANGES to a + b (the sum)
//   - return value: 0 (AWS_OP_SUCCESS)
//
// === STEP 2: FAILURE PATH ===
// When aws_add_size_checked returns AWS_OP_ERR (-1):
//   - *r: UNCHANGED (not written)
//   - return value: -1 (AWS_OP_ERR)
//   - overflow condition: b > 0 && a > (SIZE_MAX - b)
//
// === STEP 3: FRAME CONDITIONS ===
// Parameters are scalar (size_t a, size_t b) and pointer (size_t *r):
//   - a: UNCHANGED always (input only)
//   - b: UNCHANGED always (input only)
//   - *r: CHANGED on success (set to a+b), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - r must be a valid non-null pointer (precondition)
//   - On success: *r == a + b (no overflow occurred)
//   - On failure: overflow would have occurred (b > 0 && a > SIZE_MAX - b)
//   - Return value is either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1)

#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_add_size_checked_harness(void) {
    /* Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = r; /* save old value of r before call */

    /* Call the function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* Return value must be either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == 0 || result == -1);

    if (result == 0) {
        /* SUCCESS PATH */
        /* *r must equal a + b */
        assert(r == a + b);
        /* No overflow should have occurred: either b == 0 or a <= SIZE_MAX - b */
        assert(b == 0 || a <= (SIZE_MAX - b));
    } else {
        /* FAILURE PATH (result == -1) */
        /* Overflow condition must hold: b > 0 && a > SIZE_MAX - b */
        assert(b > 0 && a > (SIZE_MAX - b));
        /* *r must be unchanged */
        assert(r == old_r);
    }
}
