// === STEP 1: SUCCESS PATH ===
// When aws_add_size_checked returns AWS_OP_SUCCESS (0):
//   - *r: CHANGES to a + b (the sum)
//   - Condition: no overflow occurred (b == 0 || a <= SIZE_MAX - b)
//
// === STEP 2: FAILURE PATH ===
// When aws_add_size_checked returns AWS_OP_ERR (-1):
//   - *r: UNCHANGED (the function returns before writing to *r)
//   - Condition: overflow detected (b > 0 && a > SIZE_MAX - b)
//
// === STEP 3: FRAME CONDITIONS ===
// Parameters are scalar (size_t a, size_t b, size_t *r):
//   - a: UNCHANGED always (passed by value)
//   - b: UNCHANGED always (passed by value)
//   - *r: CHANGED on success (set to a+b), UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - r must be a valid non-null pointer (we allocate it)
//   - On success: *r == a + b
//   - On success: return value == AWS_OP_SUCCESS (0)
//   - On failure: return value == AWS_OP_ERR (-1)
//   - On failure: overflow condition holds (b > 0 && a > SIZE_MAX - b)
//   - On success: no overflow condition holds (b == 0 || a <= SIZE_MAX - b)

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/math.h>

void aws_add_size_checked_harness() {
    /* Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = r; /* save old value of r before call */

    /* Call the function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* The return value must be either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* Success path: no overflow occurred */
        /* Verify the overflow condition did NOT hold */
        assert(b == 0 || a <= (SIZE_MAX - b));
        /* Verify *r was set to a + b */
        assert(r == a + b);
    } else {
        /* Failure path: overflow was detected */
        assert(result == AWS_OP_ERR);
        /* Verify the overflow condition DID hold */
        assert(b > 0 && a > (SIZE_MAX - b));
        /* Verify *r was NOT modified */
        assert(r == old_r);
    }
}
